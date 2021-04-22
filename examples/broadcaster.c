#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#define LOG_MODULE "broadcaster_process"
#define LOG_LEVEL LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer timer;
  static char payload[] = "hello there";
  
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  while (1) {
    LOG_INFO("broadcast_process broadcasting\n");
    etimer_set(&timer, 1 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    nullnet_buf = (uint8_t*)payload;
    nullnet_len = sizeof(payload);
    NETSTACK_NETWORK.output(NULL); 
    etimer_reset(&timer);

  }
  

  LOG_INFO("broadcast_process ending\n");
  PROCESS_END();
}