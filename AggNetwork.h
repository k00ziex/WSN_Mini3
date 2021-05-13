#ifndef AGGNETWORK
#define AGGNETWORK
#include "net/netstack.h"

static const linkaddr_t dest_address_sink =     {{0x01,0x01,0x01,0x00,0x1,0x74,0x12,0x00}};
static const linkaddr_t dest_address_aggmote1 = {{0x02,0x02,0x02,0x00,0x2,0x74,0x12,0x00}};
static const linkaddr_t dest_address_aggmote2 = {{0x03,0x03,0x03,0x00,0x3,0x74,0x12,0x00}};

typedef struct AggData
{
    uint8_t AggregatorId;
    uint8_t PackageId;
    float Average;
    uint8_t Min;
    uint8_t Max;
    uint8_t Median;
    int NumMeasurements;
} AggData;

typedef struct SourceData
{
    uint8_t SourceId;
    uint8_t PackageId;
    int Value;
    // Timestamp
} SourceData;
#endif