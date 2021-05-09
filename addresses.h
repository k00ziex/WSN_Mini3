#ifndef adresses
#define adresses
#include "net/netstack.h"
static linkaddr_t dest_address_sink =     {{0x01,0x01,0x01,0x00,0x1,0x74,0x12,0x00}};
static linkaddr_t dest_address_aggmote1 = {{0x02,0x02,0x02,0x00,0x2,0x74,0x12,0x00}};
static linkaddr_t dest_address_aggmote2 = {{0x03,0x03,0x03,0x00,0x3,0x74,0x12,0x00}};
// static linkaddr_t dest_address_somemote1 = {{0x04,0x04,0x03,0x00,0x4,0x74,0x12,0x00}};
// static linkaddr_t dest_address_somemote2 = {{0x05,0x05,0x05,0x00,0x5,0x74,0x12,0x00}};
// static linkaddr_t dest_address_somemote3 = {{0x06,0x06,0x06,0x00,0x6,0x74,0x12,0x00}};
// static linkaddr_t dest_address_somemote4 = {{0x07,0x07,0x07,0x00,0x7,0x74,0x12,0x00}};
// static linkaddr_t dest_address_somemote5 = {{0x08,0x08,0x08,0x00,0x8,0x74,0x12,0x00}};

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