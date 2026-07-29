#if !defined(PARSER_H)
#define PARSER_H

#include "utils.h"
#include <cstring>

class Parser
{
private:
public:
    Parser() {}
    ~Parser() {}

    static AppState parseCommand(const char *incoming)
    {
        if (strcmp(incoming, "OTA") == 0)
            return AppState::OTA_MODE;
        else
            return AppState::NORMAL_MODE;
    }

    static const char *parseWindDirection(WindDirectionEnum direction)
    {
        switch (direction)
        {
        case WindDirectionEnum::NORTH:
            return "North";
        case WindDirectionEnum::NORTH_EAST:
            return "North East";
        case WindDirectionEnum::EAST:
            return "East";
        case WindDirectionEnum::SOUTHEAST:
            return "Southeast";
        case WindDirectionEnum::SOUTH:
            return "South";
        case WindDirectionEnum::SOUTHWEST:
            return "Southwest";
        case WindDirectionEnum::WEST:
            return "West";
        case WindDirectionEnum::NORTHWEST:
            return "Northwest";
        default:
            return "Unknown";
        }
    }

    static WindDirectionEnum parseStringWindDirection(String direction)
    {
        if (direction.equals("1"))
            return WindDirectionEnum::NORTH;
        if (direction.equals("2"))
            return WindDirectionEnum::NORTH_EAST;
        if (direction.equals("3"))
            return WindDirectionEnum::EAST;
        if (direction.equals("4"))
            return WindDirectionEnum::SOUTHEAST;
        if (direction.equals("5"))
            return WindDirectionEnum::SOUTH;
        if (direction.equals("6"))
            return WindDirectionEnum::SOUTHWEST;
        if (direction.equals("7"))
            return WindDirectionEnum::WEST;
        if (direction.equals("8"))
            return WindDirectionEnum::NORTHWEST;
    }

    static const char *parseResetReasonESP(esp_reset_reason_t reason)
    {
        switch (reason)
        {
        case ESP_RST_UNKNOWN:
            return "Unknown";
        case ESP_RST_POWERON:
            return "Power On Reset";
        case ESP_RST_EXT:
            return "External Pin Reset";
        case ESP_RST_SW:
            return "Software Reset";
        case ESP_RST_PANIC:
            return "Hardware Panic / Exception Reset";
        case ESP_RST_INT_WDT:
            return "Interrupt Watchdog Reset";
        case ESP_RST_TASK_WDT:
            return "Task Watchdog Reset";
        case ESP_RST_WDT:
            return "Other Watchdog Reset";
        case ESP_RST_DEEPSLEEP:
            return "Deep Sleep Wakeup Reset";
        case ESP_RST_BROWNOUT:
            return "Brownout Reset (Voltage Dip)";
        case ESP_RST_SDIO:
            return "SDIO Reset";
        default:
            return "Undefined Code";
        }
    }
};

#endif // PARSER_H
