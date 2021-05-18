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


#define LOG_MODULE "Source_2"
#define LOG_LEVEL LOG_LEVEL_DBG

#define FLAG_SOURCE_REMOVE_DUPLICATES false // Turn on source duplicate removal - only looks 1 back. 
#define N_PACKAGES_TO_SEND 50

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

static const uint8_t tempData[] = { 18, 23, 18, 19, 23, 22, 22, 24, 24, 22, 20, 18, 25, 25, 25, 23, 18, 18, 19, 18, 23, 19, 23, 24, 22, 21, 24, 20, 20, 24, 
                                    22, 20, 23, 19, 25, 19, 18, 18, 24, 24, 23, 24, 25, 21, 24, 21, 25, 18, 25, 22, 20, 22, 20, 19, 23, 23, 24, 20, 20, 20, 
                                    20, 25, 24, 20, 22, 24, 24, 25, 22, 18, 19, 24, 21, 23, 24, 19, 22, 19, 18, 24, 18, 21, 20, 19, 23, 25, 18, 23, 24, 25, 25, 18, 22, 24, 23, 25, 25, 25, 18, 21};

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
  aggmote_address = dest_address_aggmote1;
  cc2420_off();
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);
  
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  
  etimer_set(&timer, 10 * CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
  while (tempDataIndex < N_PACKAGES_TO_SEND) {
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
  }

  LOG_INFO("broadcast_process ending. Spent energy is:\n");
  PrintEnergestMeasurement('t', 0, 0);
  PROCESS_END();
}
