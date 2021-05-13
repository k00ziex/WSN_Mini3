
#include "shared_functions.h"
#include "stdio.h"
#include "contiki.h"
#include "sys/energest.h"
#include "inttypes.h"
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
  printf("Hej\n");
}