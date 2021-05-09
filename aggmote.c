#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "addresses.h"
#include "stdlib.h"
#define LOG_MODULE "receiver_process"
#define LOG_LEVEL LOG_LEVEL_DBG
#define PAYLOAD_SIZE 100

static char payload[PAYLOAD_SIZE];
static char myArr[25];
static int tempArr[PAYLOAD_SIZE];
static int idArr[PAYLOAD_SIZE];
static int moteIdArr[PAYLOAD_SIZE];
static int idArrIndex = 0;
static int tempArrIndex = 0;
static int moteIdArrIndex = 0;

//static char fakeData[] = { '0', '2', ';', '0', '1', ';', '2', '5', ';', 's', ';' };

void msgEventCallback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest) {
  LOG_INFO("Received message! with len: %d \n", len);
  int i = 0, protocolIndex = 0, myArrIndex = 0;
  bool moteHasSentBefore = false, dataIsRegistered = false;
  for(; i < len; i++) {
    LOG_INFO("RECEIVED: %c \n", *(char*)((uint8_t*)(data+i)));

    if (*(char*)((uint8_t*)(data + i)) == ';') {
        ++protocolIndex;
        if (protocolIndex == 1) {
            // Add MoteId to the array of MoteIds
            int moteId = atoi(myArr);
            for (int moteIdCompIndex = 0; moteIdCompIndex < moteIdArrIndex; moteIdCompIndex++) {
                if (moteIdArr[moteIdCompIndex] == moteId) {
                    moteHasSentBefore = true;
                    break;
                }
            }
            if (!moteHasSentBefore) { // If we've already registered the mote ID then we don't do it again
                LOG_INFO("Mote ID: %d\n", moteId);
                moteIdArr[moteIdArrIndex] = moteId;
                moteIdArrIndex++;
            }
        }
        else if (protocolIndex == 2) {
            // Add ID of measurement to the array of IDs
            int Id = atoi(myArr);
            if (moteHasSentBefore) {
                for (int idCompIndex = 0; idCompIndex < idArrIndex; idCompIndex++) {
                    if (idArr[idCompIndex] == Id && Id > 0) {
                        dataIsRegistered = true;
                        break;
                    }
                }
            }
            if (!dataIsRegistered) { // If we've already registrered the data and this is a duplicate, we don't register again and end early
                LOG_INFO("ID: %d\n", Id);
                idArr[idArrIndex] = Id;
                idArrIndex++;
            }
        }
        else if (protocolIndex == 3) {
            // Throw into tempArr
            int temp = atoi(myArr);
            LOG_INFO("TEMP: %d\n", temp);
            tempArr[tempArrIndex] = temp;
            tempArrIndex++;
        }
        else if (protocolIndex == 4) {
            // Timestamp stuff --> Not yet implemented (Should probably be done as protocolIndex 1, to see if the data is even relevant anymore)
        }
        // RESET myArr[]
        for (int resetIndex = 0; resetIndex < sizeof(myArr) / sizeof(myArr[0]); resetIndex++) {
            myArr[resetIndex] = 0;
        }
        myArrIndex = 0;
    }
    else {
        myArr[myArrIndex] = *(char*)((uint8_t*)(data + i));
        myArrIndex++;
    }
    if (dataIsRegistered) {
        break; // End early if we've registered the data
    }
  }

  LOG_INFO("Passing msg on\n");
  nullnet_buf = (uint8_t*)payload;
  nullnet_len = sizeof(payload);
  NETSTACK_NETWORK.output(&dest_address_sink); 
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