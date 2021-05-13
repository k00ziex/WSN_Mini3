#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "sys/energest.h"
#include "net/packetbuf.h"

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


//Char c = om der er tale om transmit reading eller receive, t for transmit
//txPower sættes i tilfælde af der er tale om transmitting
//Runtime er hvorlangtid den har stået og arbejdet dette vil i bedste tilfælde være timerens værdi. 
void energestMeasurement(char c, int8_t txPower, int runTime) {
  energest_flush(); // kan muligvis godt være at dette skal ændres så vi udregner fra den forrige måling.
  uint16_t cpuMeasurement = energest_type_time(ENERGEST_TYPE_CPU);
  uint16_t lpmMeasurement = energest_type_time(ENERGEST_TYPE_LPM);
  uint16_t txMeasurement = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  uint16_t rxMeasurement = energest_type_time(ENERGEST_TYPE_LISTEN);
  float currentConsumption;

  if(c == 't' && (txPower >= -25 && txPower <= 0)) {

    switch(txPower) {
      case -25:
        currentConsumption = 0.085;
        break;
      case -15:
        currentConsumption = 0.099;
        break;
      case -10:
        currentConsumption = 0.11;
        break;
      case -5:
        currentConsumption = 0.14;
        break;
      default:
        currentConsumption = 0.174;
    }

  } else {
    currentConsumption = 0.197;
  }

  //19,7 mA i receive mode, 3 V - det kan godt være dette lige skal undersøges nærmere da jeg var liiidt i tvivl om værdierne.
  float energyConsumption = (cpuMeasurement * currentConsumption * 3) / (ENERGEST_SECOND * runTime);
  float dutyCycle = (txMeasurement + rxMeasurement) / (cpuMeasurement + lpmMeasurement);
}



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