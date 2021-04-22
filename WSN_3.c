#include "contiki.h"
#include "sys/log.h" // Need in order to log to the console
#include <inttypes.h>
#define LOG_MODULE "WSN_MP3"
#define LOG_LEVEL LOG_LEVEL_INFO

/*---------------------------------------------------------------------------*/
PROCESS(WSN_MiniProject_3, "WSN_MiniProject_3 process");
AUTOSTART_PROCESSES(&WSN_MiniProject_3);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(WSN_MiniProject_3, ev, data)
{
	PROCESS_BEGIN();
	LOG_INFO("STARTING PROCESS.\n");
	
	LOG_INFO("PROCESS DONE.\n");
	PROCESS_END();
}