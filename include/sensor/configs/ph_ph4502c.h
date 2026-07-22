#if !defined(PH_PH4502C_SENSOR_H)
#define PH_PH4502C_SENSOR_H

#include "../base_sensor.h"
#include "ads_sensor.h"
#include "../filters/base_filter.h"

class PH4502CSensor : public BaseSensor
{
private:
    uint8_t _channelADS = 255;
    uint8_t _pinAnalog = 255;

    uint64_t _prevSampling = 0;

    const float _offset = 0;
    const float _vref = 3.3;
    const uint16_t _adcResolution = 4095;
    ADS1115Module *_ads = nullptr;

    BaseFilter *_filter = nullptr;
    BaseSensor *_temperatureSensor = nullptr;

public:
    PH4502CSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _channelADS(channelADS), _ads(ads),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    PH4502CSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog, BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _pinAnalog(pinAnalog),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    ~PH4502CSensor() override = default;

    void begin()
    {
        if (_ads == nullptr)
            pinMode(_pinAnalog, INPUT);
    }

    void update()
    {
        if (millis() - _prevSampling > 40U)
        {
            _prevSampling = millis();
            float val = _ads != nullptr ? (float)_ads->read(_channelADS) : (float)analogRead(_pinAnalog);

            if (_filter != nullptr)
                _filter->filter(val);
        }
    }

    float read() override
    {
        float val = _ads != nullptr ? (float)_ads->read(_channelADS) : (float)analogRead(_pinAnalog);
        if (_filter != nullptr)
            _filter->filter(val);
        float averageVoltage = val * (float)_vref / _adcResolution;

        float pHValue = _temperatureSensor != nullptr
                            ? (7.0 + ((2.5 - averageVoltage) / 0.18)) + (_temperatureSensor->read() - 25.0) * 0.03
                            : (7.0 + ((2.5 - averageVoltage) / 0.18));
        return pHValue;
    }
};

class MockPH4502CSensor : public BaseSensor
{
private:
    uint8_t _channelADS = 255;
    uint8_t _pinAnalog = 255;

    static const uint8_t _windowSize = 40;
    uint16_t _rawBuffer[_windowSize];
    uint8_t _arrayIndex = 0;
    uint64_t _prevSampling = 0;

    const float _offset = 0;
    const float _vref = 3.3;
    const uint16_t _adcResolution = 4095;
    ADS1115Module *_ads = nullptr;

    BaseFilter *_filter = nullptr;
    BaseSensor *_temperatureSensor = nullptr;

public:
    MockPH4502CSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _channelADS(channelADS), _ads(ads), _filter(filter), _temperatureSensor(temperatureSensor) {}

    MockPH4502CSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _pinAnalog(pinAnalog), _filter(filter), _temperatureSensor(temperatureSensor) {}

    ~MockPH4502CSensor() override = default;

    void begin()
    {
        return;
    }

    void update()
    {
        if (millis() - _prevSampling > 40U)
        {
            _prevSampling = millis();
            float val = _ads != nullptr ? (float)random(4095) : (float)random(4095);

            if (_filter != nullptr)
                _filter->filter(val);
        }
    }

    float read() override
    {
        float val = _ads != nullptr ? (float)random(4095) : (float)random(4095);
        if (_filter != nullptr)
            _filter->filter(val);
        float averageVoltage = val * (float)_vref / _adcResolution;

        float pHValue = 7.0 + ((2.5 - averageVoltage) / 0.18);
        return pHValue;
    }
};

#endif // PH_PH4502C_SENSOR_H
