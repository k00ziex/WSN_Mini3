#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "addresses.h"
#include "stdio.h"
#define LOG_MODULE "broadcaster_process"
#define LOG_LEVEL LOG_LEVEL_DBG

uint8_t getMeasurement();
uint8_t getNumDigits(uint8_t);
void payload_insert(char c);

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "broadcast_process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/

static char payload[100];
static uint8_t payloadSize; 

PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer timer;
  static char senderId = '1';
  static uint8_t packageId = 1; 
  PROCESS_BEGIN();
  LOG_INFO("broadcast_process started\n");
  while (1) {
    LOG_INFO("broadcast_process broadcasting:\n");

    etimer_set(&timer, 3 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer) );

    ++packageId;
    payloadSize = 0;
    payload_insert(senderId); // Id 
    payload_insert(';');

    uint8_t numDigits = getNumDigits(packageId);
    sprintf(payload + payloadSize, "%d", packageId);
    payloadSize += numDigits;
    payload_insert(';');

    uint8_t measurement = getMeasurement();
    numDigits = getNumDigits(measurement);

    sprintf(payload + payloadSize, "%d", measurement); // data
    payloadSize += numDigits;
    payload_insert(';');

    nullnet_buf = (uint8_t*)payload;
    nullnet_len = payloadSize * sizeof(uint8_t);
    NETSTACK_NETWORK.output(&dest_address_aggmote1); 
    LOG_INFO("\nData sent\n");
    etimer_reset(&timer);
  }

  LOG_INFO("broadcast_process ending\n");
  PROCESS_END();
}

uint8_t getMeasurement()
{
  return 69; // noice
}

void payload_insert(char c)
{
  payload[payloadSize] = c;
  payloadSize++; 
}

// https://www.javatpoint.com/count-the-number-of-digits-in-c
uint8_t getNumDigits(uint8_t n) 
{
  int count = 0;

  while(n != 0)
  {
    n = n / 10;
    count++;
  }
  return count;
}

