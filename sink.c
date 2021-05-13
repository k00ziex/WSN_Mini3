#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "sys/energest.h"
#include "net/packetbuf.h"

#include "shared_functions.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG



void msgEventCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  
  struct AggData ad;
  memcpy(&ad, data, sizeof(ad));

  LOG_INFO("I received\n");
  LOG_INFO("AggregatorID: %d\n", ad.AggregatorId);
  LOG_INFO("PackageId: %d\n", ad.PackageId);
  //print float skal bruges til at printe float værdi.
  LOG_INFO("Min Val: %d\n", ad.Min);
  LOG_INFO("Max Val: %d\n", ad.Max);
  LOG_INFO("Median: %d\n", ad.Median);
  LOG_INFO("Number of Measurements: %d\n", ad.NumMeasurements);
  
  /* LOG_INFO("Received message! with len: %d \n", len);
  int i = 0;
  for(; i < len; i++) {
    //uint8_t c = *(uint8_t*)(data+i);
    LOG_INFO("RECEIVED: %c \n", *(char*)((uint8_t*)(data+i)));
  } */
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