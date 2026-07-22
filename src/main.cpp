#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include <WebSerial.h>

#include "../utils/utils.h"
#include "../utils/parser.h"
#include "models.h"

#include "sensor/configs/modbus_sensor.h"
#include "sensor/configs/ads_sensor.h"
#include "sensor/configs/ds18b20_sensor.h"
#include "sensor/configs/bh1750_sensor.h"
#include "sensor/configs/ultrasonic_sensor.h"
#include "sensor/configs/ph_dfrobot_sensor.h"
#include "sensor/configs/tds_dfrobot_sensor.h"
#include "sensor/configs/ph_ph4502c.h"

#include "sensor/filters/moving_average.h"

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
#define ADS_CHANNEL_PH 0
#define ADS_CHANNEL_TDS 1

const char *ssid = "Subhanallah 4G";
const char *password = "muhammadnabiyullah";
const char *hostName = "aquaponic-1";
const char *blynkAuthToken = "d2oR-C4x_VlT26WNVydzEKntp-865JkX";
WiFiBlynk blynk(
    blynkAuthToken,
    ssid,
    password,
    hostName,
    [](uint8_t virtualPin, bool state)
    {
        if (virtualPin == BLYNK_WATER_PUMP_PIN)
            Serial.printf("Water Pump ON Pin %d, %d\n", PIN_WATER_PUMP, state);
        else if (virtualPin == BLYNK_LED_PIN)
            Serial.printf("LED ON Pin %d, %d\n", PIN_LED, state);
        else if (virtualPin == BLYNK_AIR_PUMP_PIN)
            Serial.printf("Air Pump ON Pin %d, %d\n", PIN_AIR_PUMP, state);
    });

MockDisplayOLED display(&Wire, ADDRESS_OLED);

ADS1115Module ads(ADS1115_ADDRESS, &Wire);

MockDS18B20Sensor waterTemperatureSensor(
    1, "Water Temperature",
    PIN_DS18);

MockPHDFRobotSensor phSensor(
    1, "PH DFRobot",
    ADS_CHANNEL_PH, &ads);

MockTDSDFRobotSensor tdsSensor(
    1, "TDS DFRobot",
    ADS_CHANNEL_TDS, &ads,
    &waterTemperatureSensor);

MockUltrasonicSensor waterLevelSensor(
    1, "Water Level",
    PIN_ECHO, PIN_TRIG);

MockBH1750Sensor lightIntensitySensor(
    1, "Light Intensity",
    &Wire, ADDRESS_BH1750);

AppState state = AppState::NORMAL_MODE;
uint64_t prevBlynkSensor = 0;
AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    if (!blynk.begin())
        Serial.println("WiFi is not connected, disabling OTA!");

    display.begin();

    // ads.begin(ADS1X15_GAIN_4096MV);
    // Wire.begin(PIN_SDA, PIN_SCL);
    tdsSensor.begin();
    phSensor.begin();
    waterTemperatureSensor.begin();
    waterLevelSensor.begin();
    lightIntensitySensor.begin();

    WebSerial.begin(&server);
    WebSerial.onMessage(
        [&](uint8_t *data, size_t len)
        {
            String d = "";
            for (uint8_t i = 0; i < len; i++)
                d += char(data[i]);
            state = Parser::parseCommand(d.c_str());
        });

    ElegantOTA.begin(&server);
    ElegantOTA.setAutoReboot(true);

    server.begin();
}

void loop()
{
    blynk.run();
    blynk.reconnect();
    ElegantOTA.loop();
    tdsSensor.update();
    phSensor.update();

    if (millis() - prevBlynkSensor > 30000)
    {
        SensorEntity sensor{
            .lightIntensity = uint16_t(lightIntensitySensor.read()),
            .waterLevel = waterLevelSensor.read(),
            .waterPH = phSensor.read(),
            .waterTemperature = waterTemperatureSensor.read(),
            .waterTDS = tdsSensor.read(),
        };
        const char *sensorString = sensor.toString();
        WebSerial.println(sensorString);

        SystemEntity system{
            .freeHeap = ESP.getFreeHeap(),
            .largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
            .minFreeHeap = ESP.getMinFreeHeap(),
            .lastResetReason = esp_reset_reason(),
        };
        const char *systemString = system.toString();
        WebSerial.println(systemString);

        blynk.send(BLYNK_AMBIENT_LIGHT_PIN, sensor.lightIntensity);
        blynk.send(BLYNK_WATER_LEVEL_PIN, sensor.waterLevel);
        blynk.send(BLYNK_WATER_PH_PIN, sensor.waterPH);
        blynk.send(BLYNK_TDS_PIN, sensor.waterTDS);
        blynk.send(BLYNK_WATER_TEMPERATURE_PIN, sensor.waterTemperature);

        char buffer[64];
        snprintf(buffer, sizeof(buffer),
                 "Water Level: %.1f", waterLevelSensor.read());
        display.showMessage(buffer);

        prevBlynkSensor = millis();
    }
}
