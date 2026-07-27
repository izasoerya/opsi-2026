#if !defined(MODELS_H)
#define MODELS_H

#include <Arduino.h>
#include "../utils/parser.h"

struct BandungPersemaianEntity
{
    const float rainFall;
    const float windSpeed;
    const char *windDirection;
    const float airTemperature;
    const float airHumidity;
    const char timestamp[9];

    const char *toString()
    {
        static char buffer[128];
        snprintf(buffer, sizeof(buffer),
                 "rain_fall: %.1f\nwind_direction: %s\nwind_speed: %.2f\nair_temperature: %.1f\nair_humidity: %.1f\ntimestamp: %s",
                 rainFall, windSpeed, windDirection, airTemperature, airHumidity, timestamp);
        return buffer;
    }
};

struct BandungPersemaianSystemEntity
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
