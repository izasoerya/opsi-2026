#if !defined(ADS_SENSOR_H)
#define ADS_SENSOR_H

#include "../base_sensor.h"
#include "reader-module/ads1115_module.h"

class ADSSensor : public BaseSensor
{
private:
    const uint8_t _channel;
    std::function<float(float)> _interceptor;

    ADS1115Module *_ads;

public:
    ADSSensor(
        unsigned char id, const char *name,
        const uint8_t channel, ADS1115Module *ads,
        std::function<float(float)> interceptor)
        : BaseSensor(id, name), _channel(channel), _ads(ads), _interceptor(interceptor) {}

    ~ADSSensor() override = default;

    float read() override
    {
        return _interceptor(_ads->read(_channel));
    }
};

class MockADSSensor : public BaseSensor
{
private:
    const uint8_t _channel;
    std::function<float(float)> _interceptor;

    ADS1115Module *_ads;

public:
    MockADSSensor(unsigned char id, const char *name,
                  const uint8_t channel, ADS1115Module *ads,
                  std::function<float(float)> interceptor)
        : BaseSensor(id, name), _channel(channel), _ads(ads), _interceptor(interceptor) {}
    ~MockADSSensor() override = default;

    float read() override
    {
        randomSeed(analogRead(A0));
        return _interceptor(random(4095));
    }
};

#endif // ADS_SENSOR_H
