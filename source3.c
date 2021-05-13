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

static const uint8_t tempData[] = { 15, 13, 13, 20, 17, 21, 21, 16, 15, 14, 16, 14, 16, 16, 14, 13, 16, 17, 15, 18, 16, 16, 19, 20, 19, 19, 17, 17, 18, 
                                    12, 14, 18, 15, 19, 14, 12, 14, 17, 21, 18, 19, 17, 17, 20, 15, 16, 14, 19, 17, 15, 19, 13, 12, 17, 18, 17, 21, 18, 
                                    17, 14, 12, 19, 13, 14, 14, 17, 17, 19, 20, 19, 21, 13, 21, 12, 14, 12, 18, 21, 17, 13, 17, 17, 13, 15, 19, 17, 13, 15, 15, 17, 12, 17, 21, 13, 19, 21, 18, 19, 15, 16};

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

