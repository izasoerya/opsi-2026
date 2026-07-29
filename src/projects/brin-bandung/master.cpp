#include <TFT_eSPI.h>
#include <ModbusClientTCPasync.h>
#include <time.h>
#include <ElegantOTA.h>
#include <WebSerial.h>

#include "models.h"
#include "display/display_tft_spi_lcd/display_tft.h"
#include "transmitter/configs/wifi_module.h"

#define TFT_SCK_PIN 8
#define TFT_MISO_PIN 20
#define TFT_MOSI_PIN 9
#define TFT_CS_PIN 5

TFT_eSPI tft = TFT_eSPI();
DisplayTFT320X240P display(tft);

const char *ssid = "NodeSensorWiFi1";
const char *password = "muhammadnabiyullah";
const char *hostName = "master-bandung-persemaian-1";
WiFiModule wifi(ssid, password, hostName);

const char *supabaseUrl = "https://pykernnkhvnssplhzcvn.supabase.co";
const char *supabasePublicKey = "sb_publishable_coDPUa845ZtfYmoBWlZlgw_eH5vsCY7";
SupabaseTransport transport = SupabaseTransport(supabaseUrl, supabasePublicKey);

AsyncWebServer server(80);

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 3600;

const char *modbusSlaveUrl = "slave-bandung-persemaian-1";
const uint16_t modbusSlavePort = 5000;
ModbusClientTCPasync *modbusClient = nullptr;
void onDataHandler(ModbusMessage response, uint32_t token);
void onErrorHandler(Error error, uint32_t token);

const uint32_t deviceId = 2;
uint32_t prevSamplingMillis = 0;
uint32_t prevSystemLoggingMillis = 0;
uint32_t counter = 0;
uint16_t sensorDatas[5];

void setup()
{
    Serial.begin(115200);

    if (wifi.begin())
        Serial.printf("Connected to: %s\n", wifi.localIP());

    ElegantOTA.begin(&server);
    ElegantOTA.setAutoReboot(true);
    server.begin();
    WebSerial.begin(&server);

    static IPAddress *modbusSlaveIP = nullptr;
    uint8_t counter = 0;
    while (modbusSlaveIP == nullptr)
    {
        modbusSlaveIP = wifi.resolveMDNS(modbusSlaveUrl);
        if (counter == 10)
            ESP.restart(); // Too long, maybe the master is the one who stuck

        counter++;
        delay(1000);
    }
    Serial.println(modbusSlaveIP->toString());
    WebSerial.printf("Slave IP: %s", modbusSlaveIP->toString());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    modbusClient = new ModbusClientTCPasync(*modbusSlaveIP, modbusSlavePort);
    modbusClient->connect();
    modbusClient->onDataHandler(&onDataHandler);
    modbusClient->onErrorHandler(&onErrorHandler);
    modbusClient->setTimeout(10000);
    modbusClient->setIdleTimeout(60000);

    SPI.begin(TFT_SCK_PIN, TFT_MISO_PIN, TFT_MOSI_PIN, TFT_CS_PIN);
    display.begin();
    display.setHeaderTitle("BANDUNG-PERSEMAIAN-1");
    display.setFooterText("v1.0.0");
    display.setContainer1("TEMPERATURE", "0.0 C", DisplayColor::ORANGE, IconType::THERMO);
    display.setContainer2("HUMIDITY", "0.0 %", DisplayColor::BLUE, IconType::DROPLET);
    display.setContainer3("WIND SPEED", "0.0 km/h", DisplayColor::TEXT, IconType::WIND);
    display.setContainer4("WIND DIR", "N", DisplayColor::YELLOW, IconType::COMPASS);
    display.setContainer5("RAINFALL", "0.0 mm", DisplayColor::TEAL, IconType::RAIN);
    display.setContainer6("COMPANY", "T4T x ZTS", DisplayColor::GREEN, IconType::COMPANY);
    display.drawLayout(); // one full paint of shells/borders/icons/labels
}

void loop()
{
    if (millis() - prevSystemLoggingMillis > 60000 * 2) // Every 2 minute
    {
        SystemLogDto systemLog{
            .deviceId = deviceId,
            .freeHeap = ESP.getFreeHeap(),
            .largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
            .minFreeHeap = ESP.getMinFreeHeap(),
            .lastResetReason = esp_reset_reason(),
        };
        Serial.println(systemLog.toString());
        WebSerial.println(systemLog.toString());

        wifi.setTransport(&transport);
        char buffer[256];
        systemLog.toJson(buffer, sizeof(buffer));
        wifi.send("system_logs", buffer);
    }

    if (millis() - prevSamplingMillis > 10000) //  Every 10 second
    {
        Error err = modbusClient->addRequest(
            (uint32_t)counter, // Token
            1, READ_HOLD_REGISTER, 0, 5);
        if (err != SUCCESS)
        {
            ModbusError e(err);
            Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
            WebSerial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
        }

        SensorDto sensor{
            .rainFall = float(sensorDatas[2] / 10.0F),
            .windSpeed = float(sensorDatas[3] / 10.0F),
            .windDirection = static_cast<WindDirectionEnum>(sensorDatas[4]),
            .airTemperature = float(sensorDatas[0] / 10.0F),
            .airHumidity = float(sensorDatas[1] / 10.0F)};
        Serial.println(sensor.toString());
        WebSerial.println(sensor.toString());

        wifi.setTransport(&transport);
        char buffer[256];
        sensor.toJson(buffer, sizeof(buffer));
        wifi.send("sensors", buffer);

        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f C", sensor.airTemperature);
        display.updateContainerValue(1, buf);

        snprintf(buf, sizeof(buf), "%.1f &RH", sensor.airHumidity);
        display.updateContainerValue(2, buf);

        snprintf(buf, sizeof(buf), "%.1f km/h", sensor.windSpeed);
        display.updateContainerValue(3, buf);

        snprintf(buf, sizeof(buf), "%s", sensor.windDirection);
        display.updateContainerValue(4, buf);

        snprintf(buf, sizeof(buf), "%.1f mm", sensor.rainFall);
        display.updateContainerValue(5, buf);

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
            Serial.println("Failed to obtain time");
        char timeHour[3];
        char timeMinute[3];
        char timeSecond[3];
        strftime(timeHour, 3, "%H", &timeinfo);
        strftime(timeMinute, 3, "%M", &timeinfo);
        strftime(timeSecond, 3, "%S", &timeinfo);
        display.setClock(atoi(timeHour), atoi(timeMinute), atoi(timeSecond));

        display.setSignalStrength(-55);
        display.refresh();
        prevSamplingMillis = millis();
        counter++;
    }
}

void onDataHandler(ModbusMessage response, uint32_t token)
{
    Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
    if (response.size() < 13)
    {
        Serial.println("Response too short, skipping");
        return;
    }

    uint16_t offset = 3;
    offset = response.get(offset, sensorDatas[0]);
    offset = response.get(offset, sensorDatas[1]);
    offset = response.get(offset, sensorDatas[2]);
    offset = response.get(offset, sensorDatas[3]);
    offset = response.get(offset, sensorDatas[4]);
    Serial.printf("Registers: %u, %u, %u, %u, %u\n", sensorDatas[0], sensorDatas[1], sensorDatas[2], sensorDatas[3], sensorDatas[4]);
}

void onErrorHandler(Error error, uint32_t token)
{
    ;
}