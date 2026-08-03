#if !defined(ADS1115_MODULE_H)
#define ADS1115_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include "ADS1115.h"

class ADS1115Module
{
private:
    TwoWire *_wire;
    ADS1115 _ads;

public:
    ADS1115Module(const uint8_t address, TwoWire *wire)
        : _ads(ADS1115(address)), _wire(wire) {}
    ~ADS1115Module() {}

    bool begin(const uint8_t gain = ADS1115_PGA_4P096)
    {
        if (!_ads.testConnection())
            return false;

        _ads.initialize();
        _ads.setMode(ADS1115_MODE_SINGLESHOT);
        _ads.setRate(ADS1115_RATE_128);
        _ads.setGain(gain);

        return true;
    }

    int16_t read(uint8_t channel)
    {
        switch (channel)
        {
        case 0:
            _ads.setMultiplexer(ADS1115_MUX_P0_NG);
            break;
        case 1:
            _ads.setMultiplexer(ADS1115_MUX_P1_NG);
            break;
        case 2:
            _ads.setMultiplexer(ADS1115_MUX_P2_NG);
            break;
        case 3:
            _ads.setMultiplexer(ADS1115_MUX_P3_NG);
            break;
        default:
            return 0;
        }
        _ads.triggerConversion();

        return _ads.getMilliVolts(false);
    }
};

#endif // ADS1115_MODULE_H