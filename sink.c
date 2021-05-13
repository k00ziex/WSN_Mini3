#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "sys/energest.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG

void printFloat(float f) {
    int32_t A = f;
    float fraction = (f-A)*100;

    if(fraction < 0) {
        fraction *= (-1);
    }
    printf("%ld.%02u, ",A, (unsigned int) fraction);
}

void energestMeasurement() {
  energest_flush(); // kan muligvis godt være at dette skal ændres så vi udregner fra den forrige måling.
  uint16_t CpuMeasurement = energest_type_time(ENERGEST_TYPE_CPU);
  uint16_t LpmMeasurement = energest_type_time(ENERGEST_TYPE_LPM);
  uint16_t TxMeasurement = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  uint16_t RxMeasurement = energest_type_time(ENERGEST_TYPE_LISTEN);

  //19,7 mA i receive mode, 3 V - det kan godt være dette lige skal undersøges nærmere da jeg var liiidt i tvivl om værdierne.
  unsigned float energyConsumption = (CpuMeasurement * 0,197 * 3) / ENERGEST_SECOND;
  unsigned float dutyCycle = (TxMeasurement + RxMeasurement) / (CpuMeasurement + LpmMeasurement);

  
}

void msgEventCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  LOG_INFO("Received message! with len: %d \n", len);
  int i = 0;
  for(; i < len; i++) {
    //uint8_t c = *(uint8_t*)(data+i);
    LOG_INFO("RECEIVED: %c \n", *(char*)((uint8_t*)(data+i)));
  }
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