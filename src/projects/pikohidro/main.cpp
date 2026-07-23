#include <Arduino.h>
#include <ElegantOTA.h>
#include <WebSerial.h>

#include "../utils/utils.h"
#include "../utils/parser.h"
#include "models.h"

#include "sensor/configs/ads_sensor.h"
#include "sensor/configs/ph_ph4502c.h"
#include "sensor/filters/moving_average.h"
#include "sensor/configs/tds_dfrobot_sensor.h"

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL69LwuKF9Y"
#define BLYNK_TEMPLATE_NAME "pikohidro"
#include "transmitter/configs/wifi_blynk.h"

#define BLYNK_WATER_PH_PIN V1
#define BLYNK_TDS_PIN V0
#define BLYNK_TURBIDITY_PIN V2
#define BLYNK_TEMPERATURE_PIN V3

#define PIN_SDA 8 // TODO: CHANGE TO ACTUAL PIN
#define PIN_SCL 9 // TODO: CHANGE TO ACTUAL PIN

#define ADDRESS_ADS1115 0x48 // TODO: CHANGE TO ACTUAL ADDRESS
#define ADDRESS_OLED 0x3C    // TODO: CHANGE TO ACTUAL ADDRESS
#define ADS_CHANNEL_TURBIDITY 0
#define ADS_CHANNEL_PH_TEMPERATURE 1
#define ADS_CHANNEL_PH 2
#define ADS_CHANNEL_TDS 3

const char *ssid = "NodeSensorWiFi1";
const char *password = "muhammadnabiyullah";
const char *hostName = "pikohidro-1";
const char *blynkAuthToken = "n6wt8wvIYJ6AoXrkmXtvI_8C0ATjMSmt";
WiFiBlynk blynk(
    blynkAuthToken,
    ssid,
    password,
    hostName,
    [](uint8_t virtualPin, bool state) {});

ADS1115Module ads(ADS1115_ADDRESS, &Wire);

MockADSSensor turbiditySensor(
    1, "Turbidity Sensor",
    ADS_CHANNEL_TURBIDITY, &ads,
    [](float value) -> float
    { return value * 3.3 / 4095.0 * 100.0; }); // TODO: CHANGE ACCRODING TO DATASHEET

MockADSSensor phTemperatureSensor(
    1, "PH Temperature Sensor",
    ADS_CHANNEL_PH_TEMPERATURE, &ads,
    [](float value) -> float
    { return value * 3.3 / 4095.0 * 100.0; }); // 10mV/°C linear sensor

TrimmedMovingAverage filterPH(20, 5);
MockPH4502CSensor phSensor(
    1, "PH Sensor",
    ADS_CHANNEL_PH, &ads,
    &filterPH, &phTemperatureSensor);

TrimmedMovingAverage filterTDS(20, 5);
MockTDSDFRobotSensor tdsSensor(
    1, "TDS Analog",
    ADS_CHANNEL_TDS, &ads,
    &filterTDS, &phTemperatureSensor);

AppState state = AppState::NORMAL_MODE;
uint64_t prevBlynkSensor = 0;
AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    if (!blynk.begin())
        Serial.println("WiFi is not connected, disabling OTA!");

    // ads.begin(ADS1X15_GAIN_4096MV);
    // Wire.begin(PIN_SDA, PIN_SCL);
    tdsSensor.begin();
    phSensor.begin();

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

    if (millis() - prevBlynkSensor > 30000)
    {
        PikohidroSensorEntity sensor{
            .waterTurbidity = turbiditySensor.read(),
            .waterPH = phSensor.read(),
            .temperature = phTemperatureSensor.read(),
            .waterTDS = tdsSensor.read(),
        };
        const char *sensorString = sensor.toString();
        WebSerial.println(sensorString);

        PikohidroSystemEntity system{
            .freeHeap = ESP.getFreeHeap(),
            .largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
            .minFreeHeap = ESP.getMinFreeHeap(),
            .lastResetReason = esp_reset_reason(),
        };
        const char *systemString = system.toString();
        WebSerial.println(systemString);

        blynk.send(BLYNK_TURBIDITY_PIN, sensor.waterTurbidity);
        blynk.send(BLYNK_WATER_PH_PIN, sensor.waterPH);
        blynk.send(BLYNK_TDS_PIN, sensor.waterTDS);
        blynk.send(BLYNK_TEMPERATURE_PIN, sensor.temperature);

        prevBlynkSensor = millis();
    }
}
