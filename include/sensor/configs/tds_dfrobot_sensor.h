#if !defined(TDS_DFROBOT_SENSOR_H)
#define TDS_DFROBOT_SENSOR_H

#include "../base_sensor.h"
#include "ads_sensor.h"

class TDSDFRobotSensor : public BaseSensor
{
private:
    uint8_t _channelADS = 255;
    uint8_t _pinAnalog = 255;

    static const uint8_t _windowSize = 30;
    uint16_t _rawBuffer[_windowSize];
    uint8_t _arrayIndex = 0;
    uint64_t _prevSampling = 0;
    float _kValue = 1.0;

    const float _vref = 3.3;
    const uint16_t _adcResolution = 4095;
    ADS1115Module *_ads = nullptr;

    BaseFilter *_filter = nullptr;
    BaseSensor *_temperatureSensor = nullptr;

public:
    TDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name),
          _channelADS(channelADS), _ads(ads),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    TDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name),
          _pinAnalog(pinAnalog),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    ~TDSDFRobotSensor() override = default;

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

        float currentTemperature = _temperatureSensor != nullptr ? _temperatureSensor->read() : 25.0;
        float compensationCoefficient = 1.0 + 0.02 * (currentTemperature - 25.0);
        float compensationVolatge = averageVoltage / compensationCoefficient;
        float tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;
        return tdsValue * _kValue;
    }
};

class MockTDSDFRobotSensor : public BaseSensor
{
private:
    uint8_t _channelADS = 255;
    uint8_t _pinAnalog = 255;

    static const uint8_t _windowSize = 30;
    uint16_t _rawBuffer[_windowSize];
    uint8_t _arrayIndex = 0;
    uint64_t _prevSampling = 0;
    float _kValue = 1.0;

    const float _vref = 3.3;
    const uint16_t _adcResolution = 4095;
    ADS1115Module *_ads = nullptr;

    BaseSensor *_temperatureSensor = nullptr;
    BaseFilter *_filter = nullptr;

public:
    MockTDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name),
          _channelADS(channelADS), _ads(ads),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    MockTDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog,
        BaseFilter *filter = nullptr, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name),
          _pinAnalog(pinAnalog),
          _filter(filter), _temperatureSensor(temperatureSensor) {}

    ~MockTDSDFRobotSensor() override = default;

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
            float val = _ads != nullptr ? float(random(4095)) : float(random(4095));

            if (_filter != nullptr)
                _filter->filter(val);
        }
    }

    float read() override
    {
        float val = _ads != nullptr ? float(random(4095)) : float(random(4095));
        if (_filter != nullptr)
            _filter->filter(val);
        float averageVoltage = val * (float)_vref / _adcResolution;

        float currentTemperature = _temperatureSensor != nullptr ? _temperatureSensor->read() : 25.0;
        float compensationCoefficient = 1.0 + 0.02 * (currentTemperature - 25.0);
        float compensationVolatge = averageVoltage / compensationCoefficient;
        float tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;
        return tdsValue * _kValue;
    }
};

#endif // TDS_DFROBOT_SENSOR_H
