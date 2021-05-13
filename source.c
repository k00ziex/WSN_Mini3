#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "stdio.h"
#include "node-id.h"
#include "net/packetbuf.h"

#define LOG_MODULE "broadcaster_process"
#define LOG_LEVEL LOG_LEVEL_DBG

#define FLAG_SOURCE_REMOVE_DUPLICATES false // Turn on source duplicate removal - only looks 1 back. 

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

static char payload[100];
static uint8_t payloadSize; 
static linkaddr_t aggmote_address; 


void nullnet_send(SourceData* data)
{
    // Set ptr type
    nullnet_buf = (uint8_t *)data;
    nullnet_len = sizeof(SourceData);
    // Copy data into buffer
    memcpy(nullnet_buf, data, sizeof(SourceData));
}

PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer timer;
  static uint8_t packageId = 1; 
  // Pick aggmote based on even ID.
  if(node_id % 2 == 0) 
  {
    aggmote_address = dest_address_aggmote1;
  } 
  else 
  {
    aggmote_address = dest_address_aggmote2;
  }
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  while (1) {
    LOG_INFO("broadcast_process broadcasting:\n");

    etimer_set(&timer, 3 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    
    // struct to send
    struct SourceData sd = {
      69,
      1,
      2
    };
  
    nullnet_send(&sd);

    NETSTACK_NETWORK.output(&aggmote_address); 
    LOG_INFO("\nData sent\n");
    etimer_reset(&timer);
  }

  LOG_INFO("broadcast_process ending\n");
  PROCESS_END();
}
