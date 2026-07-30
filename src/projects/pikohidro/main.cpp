#include <Arduino.h>
#include <ElegantOTA.h>
#include <WebSerial.h>
#include <LiquidCrystal_I2C.h>
#include "display/lcd_i2c_basic.h"

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

#define PIN_SDA 8
#define PIN_SCL 9

#define ADDRESS_ADS1115 0x48
#define ADDRESS_OLED 0x3C
#define ADS_CHANNEL_TURBIDITY 0
#define ADS_CHANNEL_PH_TEMPERATURE 1
#define ADS_CHANNEL_PH 2
#define ADS_CHANNEL_TDS 3

const uint16_t adsResolution = 32768;
const float adsRef = 4.096;

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
    {
        float voltage = value * adsRef / adsResolution;
        return pow(voltage, 2) * 1120.4 + (5742.3 * voltage) - 4352.9; // Formula from DFRobot
    });

MockADSSensor phTemperatureSensor(
    1, "PH Temperature Sensor",
    ADS_CHANNEL_PH_TEMPERATURE, &ads,
    [](float value) -> float
    { return value * adsRef / adsResolution * 100.0; }); // 10mV/°C linear sensor

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

LiquidCrystal_I2C lcd(0x27, 20, 4);

AppState state = AppState::NORMAL_MODE;
uint64_t prevBlynkSensor = 0;
AsyncWebServer server(80);

// #define CALIBRATION

void setup()
{
    Serial.begin(115200);
    // if (!blynk.begin())
    //     Serial.println("WiFi is not connected, disabling OTA!");

    Wire.begin(PIN_SDA, PIN_SCL);

    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.createChar(0, (uint8_t *)temperature_icon);
    lcd.createChar(1, (uint8_t *)ph_icon);
    lcd.createChar(2, (uint8_t *)tds_icon);
    lcd.createChar(3, (uint8_t *)turbidity_icon);

    ads.begin(ADS1X15_GAIN_4096MV);
    tdsSensor.begin();
    phSensor.begin();

    // WebSerial.begin(&server);
    // ElegantOTA.begin(&server);
    // ElegantOTA.setAutoReboot(true);

    // server.begin();
}

#ifndef CALIBRATION

void loop()
{
    // blynk.run();
    // blynk.reconnect();
    // ElegantOTA.loop();

    if (millis() - prevBlynkSensor > 2000)
    {
        PikohidroSensorEntity sensor{
            .waterTurbidity = turbiditySensor.read(),
            .waterPH = phSensor.read(),
            .temperature = phTemperatureSensor.read(),
            .waterTDS = tdsSensor.read(),
        };
        const char *sensorString = sensor.toString();
        // WebSerial.println(sensorString);

        PikohidroSystemEntity system{
            .freeHeap = ESP.getFreeHeap(),
            .largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
            .minFreeHeap = ESP.getMinFreeHeap(),
            .lastResetReason = esp_reset_reason(),
        };
        const char *systemString = system.toString();
        // WebSerial.println(systemString);

        // blynk.send(BLYNK_TURBIDITY_PIN, sensor.waterTurbidity);
        // blynk.send(BLYNK_WATER_PH_PIN, sensor.waterPH);
        // blynk.send(BLYNK_TDS_PIN, sensor.waterTDS);
        // blynk.send(BLYNK_TEMPERATURE_PIN, sensor.temperature);

        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        lcd.printf("%.2fC", sensor.temperature);

        lcd.setCursor(10, 1);
        lcd.write(byte(1));
        lcd.printf("%.2fPH", abs(sensor.waterPH));

        lcd.setCursor(0, 2);
        lcd.write(byte(2));
        lcd.printf("%.1fPPM", sensor.waterTDS);

        lcd.setCursor(10, 2);
        lcd.write(byte(3));
        lcd.printf("%.1fNTU", sensor.waterTurbidity);

        prevBlynkSensor = millis();
    }
}

#endif

#ifdef CALIBRATION

void loop()
{
    Serial.printf("Turb: %.2f\n", turbiditySensor.read());
    Serial.printf("TDS: %.2f\n", tdsSensor.read());
    Serial.printf("PH: %.2f\n", phSensor.read());
    delay(50);
}

#endif

// #include <Arduino.h>
// #include <Wire.h>

// void setup()
// {
//     Wire.begin(8, 9);
//     Serial.begin(115200);
//     Serial.println("\nI2C Scanner");
// }

// void loop()
// {
//     byte error, address;
//     int nDevices;
//     Serial.println("Scanning...");
//     nDevices = 0;
//     for (address = 1; address < 127; address++)
//     {
//         Wire.beginTransmission(address);
//         error = Wire.endTransmission();
//         if (error == 0)
//         {
//             Serial.print("I2C device found at address 0x");
//             if (address < 16)
//             {
//                 Serial.print("0");
//             }
//             Serial.println(address, HEX);
//             nDevices++;
//         }
//         else if (error == 4)
//         {
//             Serial.print("Unknow error at address 0x");
//             if (address < 16)
//             {
//                 Serial.print("0");
//             }
//             Serial.println(address, HEX);
//         }
//     }
//     if (nDevices == 0)
//     {
//         Serial.println("No I2C devices found\n");
//     }
//     else
//     {
//         Serial.println("done\n");
//     }
//     delay(5000);
// }