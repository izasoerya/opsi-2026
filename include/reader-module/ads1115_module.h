#if !defined(ADS1115_MODULE_H)
#define ADS1115_MODULE_H

#include <Arduino.h>
#include <Wire.h>
#include <ADS1X15.h>

class ADS1115Module
{
private:
    TwoWire *_wire;
    ADS1115 _ads;

public:
    ADS1115Module(const uint8_t address, TwoWire *wire)
        : _ads(ADS1115(address)), _wire(wire) {}
    ~ADS1115Module() {}

    bool begin(const uint8_t gain = ADS1X15_GAIN_4096MV)
    {
        _ads.setGain(gain);
        return _ads.begin();
    }

    int16_t read(uint8_t channel)
    {
        return _ads.readADC(channel);
    }
};

#endif // ADS1115_MODULE_H
