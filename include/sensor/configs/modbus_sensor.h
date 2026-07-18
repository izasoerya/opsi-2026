#if !defined(MODBUS_SENSOR_H)
#define MODBUS_SENSOR_H

#include <Arduino.h>
#include "SensorBuilder.h"
#include "../base_sensor.h"
#include <sensor/filters/kalman_filter.h>

class ModbusSensor : public BaseSensor
{
private:
    ModbusRTUBuilder _modbusConfig;
    BaseFilter *_filter;

public:
    ModbusSensor(
        unsigned char id, const char *name,
        Stream *stream,
        BaseFilter *filter = nullptr)
        : BaseSensor(id, name),
          _modbusConfig(ModbusRTUBuilder(*stream)),
          _filter(filter) {}

    ~ModbusSensor() override = default;

    bool begin()
    {
        ReadResult res = _modbusConfig.connect();
        if (res.isOk())
            return 1;
        return 0;
    }

    ModbusRTUBuilder &build()
    {
        return _modbusConfig;
    }

    float read() override
    {
        ReadResult res = _modbusConfig.read(0);
        if (!res.isOk())
        {
            Serial.println(res.errorMessage());
            return res.error;
        }
        else
        {
            float raw = float(_modbusConfig.read(0).value);
            if (_filter != nullptr)
                _filter->filter(raw);

            return raw;
        }
    }
};

#endif // MODBUS_SENSOR_H
