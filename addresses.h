#ifndef adresses
#define adresses
#include "net/netstack.h"
static const linkaddr_t dest_address_sink =     {{0x01,0x01,0x01,0x00,0x1,0x74,0x12,0x00}};
static const linkaddr_t dest_address_aggmote1 = {{0x02,0x02,0x02,0x00,0x2,0x74,0x12,0x00}};
static const linkaddr_t dest_address_aggmote2 = {{0x03,0x03,0x03,0x00,0x3,0x74,0x12,0x00}};

struct sourceData {
    int moteId;
    int id;
    int value;
};
struct aggregatorData {
    int aggregatorId;
    int id;
    int average;
    int min;
    int max;
    int median;
    int numberOfMeasurements;
};
#endif