#if !defined(MODELS_H)
#define MODELS_H

#include <Arduino.h>
#include "../utils/parser.h"

struct SensorEntity
{
    float acs;
    float dissolveOxygen;
    float mpx;
    float ph;
    float turbidity;
    float llt;
    float tds;
    float ec;

    const char *toString()
    {
        static char buffer[200];
        snprintf(buffer, sizeof(buffer),
                 "ACS: %.2f | DO: %.1f | MPX: %.2f | PH: %.1f | TURB: %.1f | LLT: %.1f | TDS: %.1f | EC: %.2f\n",
                 acs, dissolveOxygen, mpx, ph, turbidity, llt, tds, ec);
        return buffer;
    }
};

struct AquaponicSystemEntity
{
    uint32_t freeHeap;
    uint32_t largestFreeBlock;
    uint32_t minFreeHeap;
    esp_reset_reason_t lastResetReason;

    const char *toString()
    {
        static char buffer[128];
        snprintf(buffer, sizeof(buffer),
                 "freeHeap: %lu\nfreeBlock: %lu\nminFreeHeap: %lu\nResetReason: %s",
                 freeHeap, largestFreeBlock, minFreeHeap, Parser::parseResetReasonESP(lastResetReason));
        return buffer;
    }
};

#endif // MODELS_H
