#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "sys/energest.h"
#include "net/packetbuf.h"
#include "shared_functions.h"
#include "stdio.h"
#include "project-conf.h"

#define LOG_MODULE "Sink"
#define LOG_LEVEL LOG_LEVEL_DBG
#define N_AGG_PACKS 21
#define ARRAY_SIZE 1000
#define INVALID_TEMP_MEASUREMENT -100
#define FLAG_AGGREGATOR_END_EARLY false // Sink cannot take end early because it needs to check all received ids and package ids, too big a task.

static struct AggData aggArr[N_AGG_PACKS];
static int arrIndex = 0;
static int tenSecsWithNoPackageRcv = 0; 

void aggDataRcvCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  tenSecsWithNoPackageRcv = 0; 
  memcpy(&aggArr[arrIndex], data, sizeof(AggData));
  ++arrIndex;
}

struct idCollectionStruct {
	uint8_t MoteId;
	uint8_t PackageId;
};

static struct idCollectionStruct idArr[ARRAY_SIZE];
static int recTempArr[ARRAY_SIZE] = { [0 ... (ARRAY_SIZE - 1)] = INVALID_TEMP_MEASUREMENT };

static uint8_t idArrIndex = 0;
static uint8_t recTempIndex = 0;

static uint8_t packageId = 1;

void AggregateSourceData()
{
	int numberOfMeasurements = 0;
	uint32_t totalValue = 0;
	uint8_t minValue = 200;
	uint8_t maxValue = 0;
	for (int i = 0; i < (sizeof(recTempArr) / sizeof(recTempArr[0])); i++)
	{
		if ((int)recTempArr[i] == INVALID_TEMP_MEASUREMENT) {
			break;
		}
		if (recTempArr[i] < minValue)
		{
			minValue = recTempArr[i];
		}
		if (recTempArr[i] > maxValue)
		{
			maxValue = recTempArr[i];
		}
		totalValue += recTempArr[i];
		numberOfMeasurements++;
	}

	SelectionSort(recTempArr, numberOfMeasurements);

	float average = (float)((float)(totalValue)) / ((float)(numberOfMeasurements));
	int median = recTempArr[(numberOfMeasurements / 2)];

	LOG_INFO("Min: %d\n", minValue);
  LOG_INFO("Max: %d\n", maxValue);
  LOG_INFO("Median: %d\n", median);
  LOG_INFO("Average: "); PrintFloat(average); printf("\n");
  LOG_INFO("Num measurements: %d\n", numberOfMeasurements);

}

void resetIdArray()
{
	for (uint8_t i = 0; i < (sizeof(idArr) / sizeof(*idArr)); i++)
	{
		struct idCollectionStruct newStruct;
		idArr[i] = newStruct;
	}
}

void resetTempArray()
{
	for (int i = 0; i < (sizeof(recTempArr) / sizeof(*recTempArr)); i++)
	{
		recTempArr[i] = INVALID_TEMP_MEASUREMENT;
	}
}

void sourceDataRcvCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  bool hasRecFromMote = false;
	LOG_INFO("Received message! with len: %d \n", len);

	struct SourceData recData;
	memcpy(&recData, data, sizeof(recData));

	if (FLAG_AGGREGATOR_END_EARLY) { // End early if we've already seen this data before
		for (uint8_t idIndex = 0; idIndex < (sizeof(idArr) / sizeof(*idArr)); idIndex++)
		{
			if (idArr[idIndex].MoteId == recData.SourceId)
			{
				if (idArr[idIndex].PackageId == recData.PackageId)
				{
					return;
				}
			}
		}
	}

	// If we're at the end of one of the arrays, or out of bounds, we don't add to arrays or increase indexes
	if ((!(recTempIndex >= (sizeof(recTempArr) / sizeof(*recTempArr)))) &&
		(!(idArrIndex >= (sizeof(idArr) / sizeof(*idArr)))))
	{
		struct idCollectionStruct structToAdd = {
			.MoteId = recData.SourceId,
			.PackageId = recData.PackageId
		};
		idArr[idArrIndex] = structToAdd;
		idArrIndex++;
		recTempArr[recTempIndex] = recData.Value;
		recTempIndex++;
	}

  tenSecsWithNoPackageRcv = 0;
}




void AggregateAggData() 
{
  LOG_INFO("Sink aggregation results:\n");
  uint8_t min, max; 
  min = aggArr[0].Min; 
  max = aggArr[0].Max;
  int medianArr[N_AGG_PACKS] = {0};
  uint16_t totalMeasurements = 0;

  for(size_t j = 0; j < arrIndex; j++)
  {
    min = aggArr[j].Min < min ? aggArr[j].Min : min;
    max = aggArr[j].Max > max ? aggArr[j].Max : max;
    medianArr[j] = aggArr[j].Median;
    totalMeasurements += aggArr[j].NumMeasurements;
  }
  // weighted avg
  float avg = 0;
  for(size_t j = 0; j < arrIndex; j++)
  {
    float weight = (float)aggArr[j].NumMeasurements / (float)totalMeasurements;
    avg += aggArr[j].Average * weight; 
  }

  // Pick median
  SelectionSort(medianArr, arrIndex-1); 
  uint8_t median = medianArr[(arrIndex / 2)];

  LOG_INFO("Min: %d\n", min);
  LOG_INFO("Max: %d\n", max);
  LOG_INFO("Median: %d\n", median);
  LOG_INFO("Average: "); PrintFloat(avg); printf("\n");
  LOG_INFO("Num measurements: %d\n", totalMeasurements);

}

/*---------------------------------------------------------------------------*/
PROCESS(receiver_process, "receiver_process");
AUTOSTART_PROCESSES(&receiver_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(receiver_process, ev, data)
{
    static struct etimer timer;
    
  PROCESS_BEGIN();
  LOG_INFO("Sink started\n");
  if(NO_AGGMOTES) 
  {
    nullnet_set_input_callback(sourceDataRcvCallback);
  }
  else 
  {
    nullnet_set_input_callback(aggDataRcvCallback);
  }
  

  
  while(tenSecsWithNoPackageRcv <= 2) {
    etimer_set(&timer, 10 * CLOCK_SECOND); 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    etimer_reset(&timer);
    tenSecsWithNoPackageRcv += 1;
  }
  if(NO_AGGMOTES)
  {
    LOG_INFO("NO AGGMOTES SCENARIO\n");
    AggregateSourceData();
  }
  else 
  {
    LOG_INFO("AGGMOTES SCENARIO\n");
    AggregateAggData();
  }
  
  PrintEnergestMeasurement('t', 0, 0);
  LOG_INFO("receiver_process ending\n");
  PROCESS_END();
}