
#include "shared_functions.h"
#include "stdio.h"
#include "contiki.h"
#include "sys/energest.h"
#include <inttypes.h> 

//Char c = 't' for transmit otherwise is receive.
//txPower must be set to 
//Runtime er hvorlangtid den har stået og arbejdet dette vil i bedste tilfælde være timerens værdi. 
void PrintEnergestMeasurement(char c, int8_t txPower, int runTime) {
  energest_flush(); // kan muligvis godt være at dette skal ændres så vi udregner fra den forrige måling.
  uint64_t cpuMeasurement = energest_type_time(ENERGEST_TYPE_CPU);      // We assume that Current Consumption: MCU on, Radio off 1800 µA is for this energest. 
  uint64_t lpmMeasurement = energest_type_time(ENERGEST_TYPE_LPM);      
  uint64_t txMeasurement = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  uint64_t rxMeasurement = energest_type_time(ENERGEST_TYPE_LISTEN);

  printf("CPU clocks %" PRIu64 "\n", cpuMeasurement);
  printf("LPM clocks %" PRIu64 "\n", lpmMeasurement);
  printf("TX  clocks %" PRIu64 "\n", txMeasurement);
  printf("RX  clocks %" PRIu64 "\n", rxMeasurement);
 
  //printf("HEJ %d\n", cpuMeasurement);
  float current_tx_mode; // in mA

  if(c == 't' && (txPower >= -25 && txPower <= 0)) {

    switch(txPower) {
      case -25:
        current_tx_mode = 8.5;
        break;
      case -15:
        current_tx_mode = 9.9;
        break;
      case -10:
        current_tx_mode = 11.2;
        break;
      case -5:
        current_tx_mode = 13.9;
        break;
      default:
        current_tx_mode = 17.4;
    }

  } else {
    current_tx_mode = 17.4;
  }
  
  // https://stackoverflow.com/questions/45644277/how-to-calculate-total-energy-consumption-using-cooja
  float current_rx_mode = 19.7;
  float current_cpu_active = 0.5;
  float current_cpu_idle = 0.0026;
  uint64_t current = (  txMeasurement * current_tx_mode + rxMeasurement * current_rx_mode + 
                        cpuMeasurement * current_cpu_active + lpmMeasurement * current_cpu_idle) 
                        / RTIMER_ARCH_SECOND;
  
  uint64_t charge = current * (cpuMeasurement + lpmMeasurement) / RTIMER_ARCH_SECOND;
  uint64_t power = current * 3; // 3 Volts assumption.  TODO: current is in mA, but 3 is Volts????? fix plz
  printf("Power used : ");
  printf("%" PRIu64 "mW\n", power);
}


void PrintFloat(float f) {
    int32_t A = f;
    float fraction = (f-A)*100;

    if(fraction < 0) {
        fraction *= (-1);
    }
    printf("%ld.%02u, \n",A, (unsigned int) fraction);
}

// INSPIRED FROM: http://www.firmcodes.com/sorting-algorithms-in-c/
void SelectionSort(int arr[], int size)
{
	for (int i = 0; i < size - 1; i++)
	{
		int Imin = i;
		for (int j = i + 1; j < size; j++)
		{
			if (arr[j] < arr[Imin])
			{
				Imin = j;
			}
		}
		int temp = arr[Imin];
		arr[Imin] = arr[i];
		arr[i] = temp;
	}
}