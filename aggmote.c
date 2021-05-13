#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "stdlib.h"
#include "net/packetbuf.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG
#define ARRAY_SIZE 100
#define FLAG_AGGREGATOR_END_EARLY true
#define INVALID_TEMP_MEASUREMENT -100

static uint8_t idArr[ARRAY_SIZE] = { 0 };
static uint8_t moteIdArr[ARRAY_SIZE] = { 0 };
static int recTempArr[ARRAY_SIZE] = { [0 ... (ARRAY_SIZE - 1)] = -100 };

static uint8_t moteIdArrIndex = 0;
static uint8_t idArrIndex = 0;
static uint8_t recTempIndex = 0;

// INSPIRED FROM: http://www.firmcodes.com/sorting-algorithms-in-c/
void SelectionSort(int arr[], int size)
{
	for (int i = 0; i < size - 1; ++i)
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

AggData aggregateData()
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

	AggData aggStruct = {
		.Average = (float)((float)(totalValue)) / ((float)(numberOfMeasurements)),
		.NumMeasurements = numberOfMeasurements,
		.Max = maxValue,
		.Min = minValue,
		.Median = recTempArr[(numberOfMeasurements / 2)]
	};
	return aggStruct;
}

void resetArray(uint8_t arr[])
{
	for (uint8_t i = 0; i < (sizeof(arr) / sizeof(*arr)); i++)
	{
		arr[i] = 0;
	}
}

void resetTempArray()
{
	for (int i = 0; i < (sizeof(recTempArr) / sizeof(*recTempArr)); i++)
	{
		recTempArr[i] = -100;
	}
}


void nullnet_send(AggData* data)
{
	// Set ptr type
	nullnet_buf = (uint8_t*)data;
	nullnet_len = sizeof(AggData);
	// Copy data into buffer
	memcpy(nullnet_buf, data, sizeof(AggData));
	NETSTACK_NETWORK.output(&dest_address_sink);

	// Reset everything so we're ready to send again
	resetArray(moteIdArr);
	resetArray(idArr);
	resetTempArray();
	moteIdArrIndex = 0;
	idArrIndex = 0;
	recTempIndex = 0;
}


void msgEventCallback(const void* data, uint16_t len, const linkaddr_t* src, const linkaddr_t* dest)
{
	bool hasRecFromMote = false;
	LOG_INFO("Received message! with len: %d \n", len);

	struct SourceData test;
	memcpy(&test, data, sizeof(test));

	if (FLAG_AGGREGATOR_END_EARLY) { // End early if the flag is set to end early (Meaning no data will be aggregated)
		for (uint8_t moteIdIndex = 0; moteIdIndex < ARRAY_SIZE; moteIdIndex++)
		{
			if (moteIdArr[moteIdIndex] == test.SourceId)
			{
				hasRecFromMote = true;
				break;
			}
		}
		if (hasRecFromMote)
		{
			for (uint8_t measIdIndex = 0; measIdIndex < (sizeof(idArr) / sizeof(*idArr)); measIdIndex++)
			{
				if (idArr[measIdIndex] == test.PackageId)
				{
					return;
				}
			}
		}
	}

	// If we're at the end of one of the arrays, or out of bounds, we don't add to arrays or increase indexes
	if ((!(moteIdArrIndex >= (sizeof(moteIdArr) / sizeof(*moteIdArr)))) &&
		(!(idArrIndex >= (sizeof(idArr) / sizeof(*idArr)))) &&
		(!(recTempIndex >= (sizeof(recTempArr) / sizeof(*recTempArr)))))
	{
		moteIdArr[moteIdArrIndex] = test.SourceId;
		moteIdArrIndex++;
		idArr[idArrIndex] = test.PackageId;
		idArrIndex++;
		recTempArr[recTempIndex] = test.Value;
		recTempIndex++;
	}


	LOG_INFO("Finished receiving message.\n");
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

	while (1) {
		etimer_set(&timer, 10 * CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
		etimer_reset(&timer);

		AggData dataToSend = aggregateData();
		nullnet_send(&dataToSend);
	}

	LOG_INFO("receiver_process ending\n");
	PROCESS_END();
}