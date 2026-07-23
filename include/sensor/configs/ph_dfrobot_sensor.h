#if !defined(PH_DFROBOT_SENSOR_H)
#define PH_DFROBOT_SENSOR_H

#include "../base_sensor.h"
#include "ads_sensor.h"
#include "../filters/base_filter.h"

/**
 * @brief TDS DFROBOT Sensor Class
 *
 * @param uint8_t channelADS
 * @param ADS1115Module *ads
 * @param BaseFilter *filter
 * @param BaseSensor *temperatureSensor
 */
class PHDFRobotSensor : public BaseSensor
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
    BaseSensor *_sensor = nullptr;

public:
    PHDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *_filter = nullptr, BaseSensor *_sensor = nullptr)
        : BaseSensor(id, name), _channelADS(channelADS), _ads(ads) {}

    PHDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog,
        BaseFilter *_filter = nullptr, BaseSensor *_sensor = nullptr)
        : BaseSensor(id, name), _pinAnalog(pinAnalog) {}

    ~PHDFRobotSensor() override = default;

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

        float pHValue = 3.5 * averageVoltage + _offset;
        return pHValue;
    }
};

/**
 * @brief TDS DFROBOT Sensor Class
 *
 * @param uint8_t channelADS
 * @param ADS1115Module *ads
 * @param BaseFilter *filter
 * @param BaseSensor *temperatureSensor
 */
class MockPHDFRobotSensor : public BaseSensor
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
    BaseSensor *_sensor = nullptr;

public:
    MockPHDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads,
        BaseFilter *_filter = nullptr, BaseSensor *_sensor = nullptr)
        : BaseSensor(id, name), _channelADS(channelADS), _ads(ads) {}

    MockPHDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog,
        BaseFilter *_filter = nullptr, BaseSensor *_sensor = nullptr)
        : BaseSensor(id, name), _pinAnalog(pinAnalog) {}

    ~MockPHDFRobotSensor() override = default;

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

        float pHValue = 3.5 * averageVoltage + _offset;
        return pHValue;
    }
};

#endif // PH_DFROBOT_SENSOR_H
