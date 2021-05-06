#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "addresses.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG

static char payload[] = "hello there"; // HARDCODED MESSAGE PASSING :) 


void msgEventCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  LOG_INFO("Received message! with len: %d \n", len);
  int i = 0;
  for(; i < len; i++) {
    //uint8_t c = *(uint8_t*)(data+i);
    LOG_INFO("RECEIVED: %c \n", *(char*)((uint8_t*)(data+i)));
  }
  LOG_INFO("Passing msg on\n");
  nullnet_buf = (uint8_t*)payload;
  nullnet_len = sizeof(payload);
  NETSTACK_NETWORK.output(&dest_address_sink); 
}

/*---------------------------------------------------------------------------*/
PROCESS(receiver_process, "receiver_process");
AUTOSTART_PROCESSES(&receiver_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(receiver_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();
  LOG_INFO("receiver_process started\n");
 
  nullnet_set_input_callback(msgEventCallback);

  
  while(1) {
    etimer_set(&timer, 10 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    etimer_reset(&timer);
  }

  LOG_INFO("receiver_process ending\n");
  PROCESS_END();
}