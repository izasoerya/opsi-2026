#if !defined(BH1750_SENSOR_H)
#define BH1750_SENSOR_H

#include <BH1750.h>
#include "../base_sensor.h"

class BH1750Sensor : public BaseSensor
{
private:
    TwoWire *_wire;
    uint8_t _address;
    BH1750 _lux;

public:
    BH1750Sensor(
        unsigned char id, const char *name,
        TwoWire *wire, uint8_t address)
        : BaseSensor(id, name), _wire(wire), _address(address) {}

    ~BH1750Sensor() override = default;

    bool begin()
    {
        if (!_lux.begin(_address))
            return false;
        _lux.powerOn();
        _lux.setMode(BH1750_CONTINOUS_H_RES_MOD);
        return true;
    }

    float read() override
    {
        if (_lux.isMeasurementReady())
        {
            return _lux.readValue();
        }
    }
};

class MockBH1750Sensor : public BaseSensor
{
private:
    TwoWire *_wire;
    uint8_t _address;
    BH1750 _lux;

public:
    MockBH1750Sensor(
        unsigned char id, const char *name,
        TwoWire *wire, uint8_t address)
        : BaseSensor(id, name), _wire(wire), _address(address) {}

    ~MockBH1750Sensor() override = default;

    bool begin()
    {
        return true;
    }

    float read() override
    {
        return random(1000);
    }
};

#endif // BH1750_SENSOR_H
