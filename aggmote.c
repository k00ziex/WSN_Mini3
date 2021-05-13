#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "node-id.h"
#include "stdlib.h"
#include "net/packetbuf.h"
#include "shared_functions.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG
#define ARRAY_SIZE 100
#define FLAG_AGGREGATOR_END_EARLY true
#define INVALID_TEMP_MEASUREMENT -100
#define NO_AGGREGATION false

struct idCollectionStruct {
	uint8_t MoteId;
	uint8_t PackageId;
};

static struct idCollectionStruct idArr[ARRAY_SIZE];
static int recTempArr[ARRAY_SIZE] = { [0 ... (ARRAY_SIZE - 1)] = -100 };

static uint8_t idArrIndex = 0;
static uint8_t recTempIndex = 0;

static uint8_t packageId = 1;

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
		.AggregatorId = node_id,
		.PackageId = packageId,
		.Average = (float)((float)(totalValue)) / ((float)(numberOfMeasurements)),
		.NumMeasurements = numberOfMeasurements,
		.Max = maxValue,
		.Min = minValue,
		.Median = recTempArr[(numberOfMeasurements / 2)]
	};
	packageId++;
	return aggStruct;
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
		recTempArr[i] = -100;
	}
}


void nullnet_send(AggData* data)
{
	if (data->NumMeasurements > 0) {
		// Set ptr type
		nullnet_buf = (uint8_t*)data;
		nullnet_len = sizeof(AggData);
		// Copy data into buffer
		memcpy(nullnet_buf, data, sizeof(AggData));
		NETSTACK_NETWORK.output(&dest_address_sink);

		// Reset everything so we're ready to send again
		resetIdArray();
		resetTempArray();
		idArrIndex = 0;
		recTempIndex = 0;
	}
}

void nullnet_send_no_agg(SourceData* data)
{
	// Set ptr type
	nullnet_buf = (uint8_t*)data;
	nullnet_len = sizeof(SourceData);
	// Copy data into buffer
	memcpy(nullnet_buf, data, sizeof(SourceData));
	NETSTACK_NETWORK.output(&dest_address_sink);
}


void msgEventCallback(const void* data, uint16_t len, const linkaddr_t* src, const linkaddr_t* dest)
{
	if (NO_AGGREGATION) {
		struct SourceData recData;
		memcpy(&recData, data, sizeof(recData));
		nullnet_send_no_agg(&recData);
	}
	else {
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