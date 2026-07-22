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
    BaseSensor *_temperatureSensor = nullptr;
    ADS1115Module *_ads = nullptr;

    uint16_t _getMedianNum(uint16_t bArray[], uint8_t iFilterLen)
    {
        uint16_t bTab[iFilterLen];
        for (byte i = 0; i < iFilterLen; i++)
            bTab[i] = bArray[i];
        uint16_t i, j, bTemp;
        for (j = 0; j < iFilterLen - 1; j++)
        {
            for (i = 0; i < iFilterLen - j - 1; i++)
            {
                if (bTab[i] > bTab[i + 1])
                {
                    bTemp = bTab[i];
                    bTab[i] = bTab[i + 1];
                    bTab[i + 1] = bTemp;
                }
            }
        }
        if ((iFilterLen & 1) > 0)
            bTemp = bTab[(iFilterLen - 1) / 2];
        else
            bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
        return bTemp;
    }

public:
    TDSDFRobotSensor(
        unsigned char id, const char *name,
        BaseSensor *temperatureSensor, uint8_t channelADS, ADS1115Module *ads)
        : BaseSensor(id, name), _temperatureSensor(temperatureSensor), _channelADS(channelADS), _ads(ads) {}

    TDSDFRobotSensor(
        unsigned char id, const char *name,
        BaseSensor *temperatureSensor, uint8_t pinAnalog)
        : BaseSensor(id, name), _pinAnalog(pinAnalog) {}

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
            _rawBuffer[_arrayIndex] = _ads != nullptr ? _ads->read(_channelADS) : analogRead(_pinAnalog);
            _arrayIndex++;
            if (_arrayIndex == _windowSize)
                _arrayIndex = 0;
        }
    }

    float read() override
    {
        uint16_t analogBufferTemp[_windowSize];
        for (uint8_t copyIndex = 0; copyIndex < _windowSize; copyIndex++)
            analogBufferTemp[copyIndex] = _rawBuffer[copyIndex];
        float averageVoltage = _getMedianNum(analogBufferTemp, _windowSize) * (float)_vref / _adcResolution;

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
    BaseSensor *_temperatureSensor = nullptr;
    ADS1115Module *_ads = nullptr;

    uint16_t _getMedianNum(uint16_t bArray[], uint8_t iFilterLen)
    {
        uint16_t bTab[iFilterLen];
        for (byte i = 0; i < iFilterLen; i++)
            bTab[i] = bArray[i];
        uint16_t i, j, bTemp;
        for (j = 0; j < iFilterLen - 1; j++)
        {
            for (i = 0; i < iFilterLen - j - 1; i++)
            {
                if (bTab[i] > bTab[i + 1])
                {
                    bTemp = bTab[i];
                    bTab[i] = bTab[i + 1];
                    bTab[i + 1] = bTemp;
                }
            }
        }
        if ((iFilterLen & 1) > 0)
            bTemp = bTab[(iFilterLen - 1) / 2];
        else
            bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
        return bTemp;
    }

public:
    MockTDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t channelADS, ADS1115Module *ads, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _temperatureSensor(temperatureSensor), _channelADS(channelADS), _ads(ads) {}

    MockTDSDFRobotSensor(
        unsigned char id, const char *name,
        uint8_t pinAnalog, BaseSensor *temperatureSensor = nullptr)
        : BaseSensor(id, name), _pinAnalog(pinAnalog) {}

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
            _rawBuffer[_arrayIndex] = _ads != nullptr ? random(4095) : random(4095);
            _arrayIndex++;
            if (_arrayIndex == _windowSize)
                _arrayIndex = 0;
        }
    }

    float read() override
    {
        uint16_t analogBufferTemp[_windowSize];
        for (uint8_t copyIndex = 0; copyIndex < _windowSize; copyIndex++)
            analogBufferTemp[copyIndex] = _rawBuffer[copyIndex];
        float averageVoltage = _getMedianNum(analogBufferTemp, _windowSize) * (float)_vref / _adcResolution;

        float currentTemperature = _temperatureSensor != nullptr ? _temperatureSensor->read() : 25.0;
        float compensationCoefficient = 1.0 + 0.02 * (currentTemperature - 25.0);
        float compensationVolatge = averageVoltage / compensationCoefficient;
        float tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;
        return tdsValue * _kValue;
    }
};

#endif // TDS_DFROBOT_SENSOR_H
