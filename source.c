//**** SHARED CODE FOR ALL SOURCE MOTES -> If fix stuff do it in here. 
// THIS IS NOT SUPPOSED TO BE USED AS A MOTE :)

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "AggNetwork.h"
#include "stdio.h"
#include "node-id.h"
#include "net/packetbuf.h"
#include "shared_functions.h"

#define LOG_MODULE "broadcaster_process"
#define LOG_LEVEL LOG_LEVEL_DBG

#define FLAG_SOURCE_REMOVE_DUPLICATES false // Turn on source duplicate removal - only looks 1 back. 

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

static const uint8_t tempData[] = { 26, 22, 25, 30, 20, 16, 27, 20, 24, 17, 16, 17, 28, 19, 21, 25, 28, 27, 20, 18, 25, 15, 19, 25, 24, 20, 30, 16, 26, 17, 21, 28, 20, 16, 23, 19, 17, 26, 
                            22, 17, 16, 18, 17, 20, 30, 23, 18, 21, 25, 23, 24, 28, 21, 26, 17, 15, 20, 28, 21, 30, 24, 20, 29, 29, 28, 19, 19, 27, 26, 22, 30, 22, 30, 22, 23, 27, 
                            21, 22, 17, 27, 24, 25, 27, 18, 28, 29, 19, 26, 20, 19, 17, 25, 23, 28, 19, 17, 21, 22, 27, 17};

static int tempDataIndex = 0;
static uint8_t packageId = 1; 

uint8_t GetNextSourceData() 
{
  uint8_t val = tempData[tempDataIndex];
  ++packageId;
  ++tempDataIndex;
  return val;
}

static linkaddr_t aggmote_address; 
void nullnet_send(SourceData* data)
{
    // Set ptr type
    nullnet_buf = (uint8_t *)data;
    nullnet_len = sizeof(SourceData);
    // Copy data into buffer
    memcpy(nullnet_buf, data, sizeof(SourceData));
    NETSTACK_NETWORK.output(&aggmote_address); 
    LOG_INFO("\nData sent\n");
}

PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer timer;
  energestMeasurement('t',  0,  10);
  // Pick aggmote based on even ID.
  if(node_id % 2 == 0) 
  {
    aggmote_address = dest_address_aggmote1;
  } 
  else 
  {
    aggmote_address = dest_address_aggmote2;
  }
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  while (1) {
    LOG_INFO("broadcast_process broadcasting:\n");

    etimer_set(&timer, 1 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );
    
    uint8_t val = GetNextSourceData();

    // struct to send
    struct SourceData sd;
    sd.SourceId = node_id;
    sd.PackageId = packageId;
    sd.Value = val;
    
    NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);

    nullnet_send(&sd);
    
    etimer_reset(&timer);
    // End of life for our mote.
    if(tempDataIndex >= 99) 
    {
      break;
    }
  }

  LOG_INFO("broadcast_process ending\n");
  PROCESS_END();
}
