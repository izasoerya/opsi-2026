#if !defined(ULTRASONIC_SENSOR_H)
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>
#include "../base_sensor.h"

class UltrasonicSensor : public BaseSensor
{
private:
    uint8_t _echoPin;
    uint8_t _trigPin;

public:
    UltrasonicSensor(
        unsigned char id, const char *name,
        uint8_t echoPin, uint8_t trigPin)
        : BaseSensor(id, name),
          _echoPin(echoPin), _trigPin(trigPin) {}

    ~UltrasonicSensor() override = default;

    void begin()
    {
        pinMode(_echoPin, INPUT);
        pinMode(_trigPin, OUTPUT);
    }

    float read() override
    {
        digitalWrite(_trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(_trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(_trigPin, LOW);

        float cm = (pulseIn(_echoPin, HIGH) / 2) / 29.1;
        return cm;
    }
};

class MockUltrasonicSensor : public BaseSensor
{
private:
    uint8_t _echoPin;
    uint8_t _trigPin;

public:
    MockUltrasonicSensor(
        unsigned char id, const char *name,
        uint8_t echoPin, uint8_t trigPin)
        : BaseSensor(id, name),
          _echoPin(echoPin), _trigPin(trigPin) {}

    ~MockUltrasonicSensor() override = default;

    void begin()
    {
        return;
    }

    float read() override
    {
        return random(400);
    }
};

#endif // ULTRASONIC_SENSOR_H
