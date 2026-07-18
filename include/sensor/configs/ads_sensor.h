#if !defined(ADS_SENSOR_H)
#define ADS_SENSOR_H

#include "../base_sensor.h"
#include "../ads1115_module/ads1115_module.h"

class ADSSensor : public BaseSensor
{
private:
    const uint8_t _channel;
    ADS1115Module *_ads;

public:
    ADSSensor(unsigned char id, const char *name, const uint8_t channel, ADS1115Module *ads)
        : BaseSensor(id, name), _channel(channel), _ads(ads) {}
    ~ADSSensor() override = default;

    float read() override
    {
        return _ads->read(_channel);
    }
};

class MockADSSensor : public BaseSensor
{
private:
    const uint8_t _channel;
    ADS1115Module *_ads;

public:
    MockADSSensor(unsigned char id, const char *name, const uint8_t channel, ADS1115Module *ads)
        : BaseSensor(id, name), _channel(channel), _ads(ads) {}
    ~MockADSSensor() override = default;

    float read() override
    {
        randomSeed(analogRead(A0));
        return random(100);
    }
};

#endif // ADS_SENSOR_H
