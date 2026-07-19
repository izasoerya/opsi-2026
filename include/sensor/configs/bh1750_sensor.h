#if !defined(BH1750_SENSOR_H)
#define BH1750_SENSOR_H

#include <ArtronShop_BH1750.h>
#include "../base_sensor.h"

class BH1750Sensor : public BaseSensor
{
private:
    TwoWire *_wire;
    uint8_t _address;
    ArtronShop_BH1750 _lux;

public:
    BH1750Sensor(
        unsigned char id, const char *name,
        TwoWire *wire, uint8_t address)
        : BaseSensor(id, name), _wire(wire), _address(address), _lux(ArtronShop_BH1750(_address, _wire)) {}

    ~BH1750Sensor() override = default;

    bool begin()
    {
        if (!_lux.begin())
            return false;
        return true;
    }

    float read() override
    {
        return _lux.light();
    }
};

class MockBH1750Sensor : public BaseSensor
{
private:
    TwoWire *_wire;
    uint8_t _address;
    ArtronShop_BH1750 _lux;

public:
    MockBH1750Sensor(
        unsigned char id, const char *name,
        TwoWire *wire, uint8_t address)
        : BaseSensor(id, name), _wire(wire), _address(address), _lux(ArtronShop_BH1750(_address, _wire)) {}

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
