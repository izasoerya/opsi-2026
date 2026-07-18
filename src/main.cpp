#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "sensor/configs/modbus_sensor.h"
#include "sensor/configs/ads_sensor.h"
#include "sensor/configs/ds18b20_sensor.h"
#include "sensor/configs/bh1750_sensor.h"
#include "sensor/configs/ultrasonic_sensor.h"

#include "transmitter/configs/lora.h"
#include "transmitter/configs/wifi_module.h"

#include "display/display_oled.h"

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6MXdBYHV3"
#define BLYNK_TEMPLATE_NAME "Aquaponic"
#include "transmitter/configs/wifi_blynk.h"

#define BLYNK_AMBIENT_LIGHT_PIN V0
#define BLYNK_WATER_LEVEL_PIN V1
#define BLYNK_WATER_PH_PIN V2
#define BLYNK_WATER_TEMPERATURE_PIN V3
#define BLYNK_TDS_PIN V4
#define BLYNK_WATER_PUMP_PIN V5
#define BLYNK_LED_PIN V6
#define BLYNK_AIR_PUMP_PIN V7

#define PIN_ECHO 5        // TODO: CHANGE TO ACTUAL PIN
#define PIN_TRIG 6        // TODO: CHANGE TO ACTUAL PIN
#define PIN_DS18 7        // TODO: CHANGE TO ACTUAL PIN
#define PIN_SDA 8         // TODO: CHANGE TO ACTUAL PIN
#define PIN_SCL 9         // TODO: CHANGE TO ACTUAL PIN
#define PIN_WATER_PUMP 15 // TODO: CHANGE TO ACTUAL PIN
#define PIN_LED 16        // TODO: CHANGE TO ACTUAL PIN
#define PIN_AIR_PUMP 17   // TODO: CHANGE TO ACTUAL PIN

#define ADDRESS_BH1750 0x23  // TODO: CHANGE TO ACTUAL ADDRESS
#define ADDRESS_ADS1115 0x48 // TODO: CHANGE TO ACTUAL ADDRESS
#define ADDRESS_OLED 0x3C    // TODO: CHANGE TO ACTUAL ADDRESS

const char *ssid = "Subhanallah 4G";
const char *password = "muhammadnabiyullah";
// const char *hostName = "ws-brin-bandung-1";
const char *blynkAuthToken = "d2oR-C4x_VlT26WNVydzEKntp-865JkX";
WiFiBlynk blynk(
    blynkAuthToken,
    ssid,
    password,
    [](uint8_t virtualPin, bool state)
    {
        if (virtualPin == BLYNK_WATER_PUMP_PIN)
            Serial.printf("Water Pump ON Pin %d, %d\n", PIN_WATER_PUMP, state);
        else if (virtualPin == BLYNK_LED_PIN)
            Serial.printf("LED ON Pin %d, %d\n", PIN_LED, state);
        else if (virtualPin == BLYNK_AIR_PUMP_PIN)
            Serial.printf("Air Pump ON Pin %d, %d\n", PIN_AIR_PUMP, state);
    });

// MockDisplayOLED display(&Wire, ADDRESS_OLED);

OneWire onewire(PIN_DS18);
ADS1115Module ads(ADS1115_ADDRESS, &Wire);

MockDS18B20Sensor waterTemperatureSensor(1, "Water Temp Analog", &onewire);
MockADSSensor waterPhSensor(1, "Water Ph Analog", 1, &ads);
MockADSSensor tdsSensor(1, "TDS Analog", 1, &ads);
MockUltrasonicSensor waterLevelSensor(1, "Water Level Analog", PIN_ECHO, PIN_TRIG);
MockBH1750Sensor lightIntensitySensor(1, "Light Intensity Analog", &Wire, ADDRESS_BH1750);

uint64_t prevBlynkSensor = 0;

void setup()
{
    Serial.begin(115200);
    blynk.begin();

    // display.begin();

    // ads.begin(ADS1X15_GAIN_4096MV);
    // Wire.begin(PIN_SDA, PIN_SCL);
    waterTemperatureSensor.begin();
    waterLevelSensor.begin();
    lightIntensitySensor.begin();
}

void loop()
{
    blynk.run();

    if (millis() - prevBlynkSensor > 30000)
    {
        blynk.send(BLYNK_AMBIENT_LIGHT_PIN, lightIntensitySensor.read());
        blynk.send(BLYNK_WATER_LEVEL_PIN, waterLevelSensor.read());
        blynk.send(BLYNK_WATER_PH_PIN, waterPhSensor.read());
        blynk.send(BLYNK_WATER_TEMPERATURE_PIN, waterTemperatureSensor.read());
        blynk.send(BLYNK_TDS_PIN, tdsSensor.read());

        // char buffer[64];
        // snprintf(buffer, sizeof(buffer),
        //          "Water Level: %.1f", waterLevelSensor.read());
        // display.showMessage(buffer);

        prevBlynkSensor = millis();
    }
}
