
#include "shared_functions.h"
#include "stdio.h"
#include "contiki.h"
#include "sys/energest.h"
#include <inttypes.h> 

//Char c = om der er tale om transmit reading eller receive, t for transmit
//txPower sættes i tilfælde af der er tale om transmitting
//Runtime er hvorlangtid den har stået og arbejdet dette vil i bedste tilfælde være timerens værdi. 
void PrintEnergestMeasurement(char c, int8_t txPower, int runTime) {
  energest_flush(); // kan muligvis godt være at dette skal ændres så vi udregner fra den forrige måling.
  uint64_t cpuMeasurement = energest_type_time(ENERGEST_TYPE_CPU);
  uint64_t lpmMeasurement = energest_type_time(ENERGEST_TYPE_LPM);
  uint64_t txMeasurement = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  uint64_t rxMeasurement = energest_type_time(ENERGEST_TYPE_LISTEN);

  printf("VALL :) %" PRIu64 "\n", cpuMeasurement);
     printf("VALL :) %" PRIu64 "\n", lpmMeasurement);
  printf("VALL :) %" PRIu64 "\n", txMeasurement);
  printf("VALL :) %" PRIu64 "\n", rxMeasurement);
 
  //printf("HEJ %d\n", cpuMeasurement);
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
  printf("Energy consumption: ");
  PrintFloat(energyConsumption);
  printf("Duty cycle: ");
  PrintFloat(dutyCycle);
}


void PrintFloat(float f) {
    int32_t A = f;
    float fraction = (f-A)*100;

    if(fraction < 0) {
        fraction *= (-1);
    }
    printf("%ld.%02u, \n",A, (unsigned int) fraction);
}