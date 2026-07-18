#if !defined(DS18B20_SENSOR_H)
#define DS18B20_SENSOR_H

#include "../base_sensor.h"
#include "DS18B20.h"

class DS18B20Sensor : public BaseSensor
{
private:
    OneWire *_onewire;
    DS18B20 _sensor;

public:
    DS18B20Sensor(unsigned char id, const char *name,
                  OneWire *onewire)
        : BaseSensor(id, name), _onewire(onewire), _sensor(_onewire) {}

    ~DS18B20Sensor() override = default;

    bool begin()
    {
        if (!_sensor.begin())
            return false;
        _sensor.setResolution(10);
        return true;
    }

    float read() override
    {
        _sensor.requestTemperatures();
        return _sensor.getTempC();
    }
};

class MockDS18B20Sensor : public BaseSensor
{
private:
    OneWire *_onewire;
    DS18B20 _sensor;

public:
    MockDS18B20Sensor(unsigned char id, const char *name,
                      OneWire *onewire)
        : BaseSensor(id, name), _onewire(onewire), _sensor(_onewire) {}

    ~MockDS18B20Sensor() override = default;

    bool begin()
    {
        return true;
    }

    float read() override
    {
        return random(125);
    }
};

#endif // DS18B20_SENSOR_H
