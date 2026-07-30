#if !defined(MODELS_H)
#define MODELS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "../utils/parser.h"

struct SensorDto
{
    const uint32_t deviceId;
    const float rainFall;
    const float windSpeed;
    const WindDirectionEnum windDirection;
    const float airTemperature;
    const float airHumidity;
    const char timestamp[9];

    const char *toString()
    {
        static char buffer[128];
        snprintf(buffer, sizeof(buffer),
                 "rain_fall: %.1f\nwind_direction: %s\nwind_speed: %.2f\nair_temperature: %.1f\nair_humidity: %.1f",
                 rainFall, Parser::parseWindDirection(windDirection), windSpeed, airTemperature, airHumidity);
        return buffer;
    }

    size_t toJson(char *out, size_t outSize) const
    {
        JsonDocument doc;
        doc["device_id"] = deviceId;
        doc["rain_fall"] = rainFall;
        doc["wind_direction"] = Parser::parseWindDirection(windDirection);
        doc["wind_speed"] = windSpeed;
        doc["temperature"] = airTemperature;
        doc["humidity"] = airHumidity;

        return serializeJson(doc, out, outSize);
    }
};

struct SystemLogDto
{
    uint32_t deviceId;
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

    size_t toJson(char *out, size_t outSize)
    {
        JsonDocument doc;
        doc["device_id"] = deviceId;
        doc["free_heap"] = freeHeap;
        doc["largest_free_block"] = largestFreeBlock;
        doc["min_free_heap"] = minFreeHeap;
        doc["reset_reason"] = Parser::parseResetReasonESP(lastResetReason);

        return serializeJson(doc, out, outSize);
    }
};

#endif // MODELS_H
