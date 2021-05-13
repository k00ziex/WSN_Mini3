#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "stdio.h"
#include "node-id.h"
#include "net/packetbuf.h"
#include "shared_functions.h"
#include "cc2420.h"


#define LOG_MODULE "broadcaster_process"
#define LOG_LEVEL LOG_LEVEL_DBG

#define FLAG_SOURCE_REMOVE_DUPLICATES false // Turn on source duplicate removal - only looks 1 back. 

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

static const uint8_t tempData[] = { 8, 11, 11, 10, 15, 12, 15, 15, 10, 8, 8, 13, 11, 16, 11, 9, 12, 16, 13, 11, 10, 15, 16, 11, 8, 9, 13, 9, 10, 12, 11, 10, 12, 10, 
                                    8, 13, 9, 15, 11, 10, 14, 14, 9, 9, 9, 13, 15, 12, 9, 14, 16, 9, 15, 9, 15, 15, 12, 12, 16, 8, 12, 13, 14, 10, 8, 9, 14, 16, 11, 
                                    16, 10, 9, 11, 13, 13, 12, 10, 9, 9, 9, 14, 12, 9, 14, 9, 15, 12, 12, 9, 15, 12, 13, 11, 14, 8, 13, 15, 12, 13, 10};

static int tempDataIndex = 0;
static uint8_t packageId = 1; 

uint8_t GetNextSourceData() 
{
  uint8_t val = tempData[tempDataIndex];
  ++packageId;
  ++tempDataIndex;
  return val;
}

static linkaddr_t aggmote_address; 
void nullnet_send(SourceData* data)
{
    // Set ptr type
    nullnet_buf = (uint8_t *)data;
    nullnet_len = sizeof(SourceData);
    // Copy data into buffer
    memcpy(nullnet_buf, data, sizeof(SourceData));
    cc2420_on();
    NETSTACK_NETWORK.output(&aggmote_address); 
    cc2420_off();
}

PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer timer;
  // Source 1 & 2 contacts aggmote1
  aggmote_address = dest_address_aggmote2;
  cc2420_off();
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);
  
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  
  while (1) {
    LOG_INFO("broadcast_process broadcasting:\n");

    etimer_set(&timer, 1 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    
    uint8_t val = GetNextSourceData();

    // struct to send
    struct SourceData sd;
    sd.SourceId = node_id;
    sd.PackageId = packageId;
    sd.Value = val;

    nullnet_send(&sd);
    etimer_reset(&timer);
    // End of life for our mote.
    if(tempDataIndex >= 99) 
    {
      break;
    }
  }

  LOG_INFO("broadcast_process ending. Spent energy is:\n");
  PrintEnergestMeasurement('t', 0, 0);
  PROCESS_END();
}
