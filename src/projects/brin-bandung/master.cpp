#include <TFT_eSPI.h>
#include <ModbusClientTCPasync.h>
#include <time.h>
#include <ElegantOTA.h>
#include <WebSerial.h>
#include <WireGuard-ESP32.h>

#include "config.h" // .env
#include "models.h"
#include "display/display_tft_spi_lcd/display_tft.h"
#include "transmitter/configs/wifi_module.h"

#define TFT_SCK_PIN 8
#define TFT_MISO_PIN 20
#define TFT_MOSI_PIN 9
#define TFT_CS_PIN 5

TFT_eSPI tft = TFT_eSPI();
DisplayTFT480X320P display(tft);

const char *ssid = "NodeSensorWiFi1";
const char *password = "muhammadnabiyullah";
const char *hostName = "master-bandung-persemaian-1";
WiFiModule wifi(ssid, password, hostName);

const char *supabaseUrl = "https://pykernnkhvnssplhzcvn.supabase.co";
const char *supabasePublicKey = "sb_publishable_coDPUa845ZtfYmoBWlZlgw_eH5vsCY7";
SupabaseTransport transport = SupabaseTransport(supabaseUrl, supabasePublicKey);

AsyncWebServer server(80);
WireGuard wg;

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
uint32_t stampDataModbusCounter = 0;
uint8_t errorTransactionModbusCounter = 0;
uint16_t sensorDatas[5];

void setup()
{
    Serial.begin(115200);

    if (wifi.begin())
        Serial.printf("Connected to: %s\n", wifi.localIP());

    ElegantOTA.begin(&server);
    ElegantOTA.setAutoReboot(true);
    WebSerial.begin(&server);
    server.begin();

    uint8_t retryCounter = 0;
    struct tm timeinfo;
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.google.com");
    while (!getLocalTime(&timeinfo) && retryCounter < 20)
    {
        Serial.print(".");
        delay(500);
        if (retryCounter >= 20)
        {
            Serial.println("\nNTP Sync Failed! Restarting...");
            ESP.restart(); // Critical: WireGuard handshake will fail without correct time
        }
        retryCounter++;
    }

    IPAddress modbusSlaveIP;
    IPAddress resolved = wifi.resolveMDNS(modbusSlaveUrl);
    if (resolved != IPAddress(0, 0, 0, 0))
        modbusSlaveIP = resolved;
    else
        ESP.restart(); // Master cant reach slave, no data will be feeded. Just restart

    IPAddress wgLocalIP;
    wgLocalIP.fromString(WG_DEVICE_MASTER_LOCAL_IP_1);
    Serial.printf("wg ip: %s\n", wgLocalIP.toString());
    bool wgOk = wg.begin(wgLocalIP, WG_DEVICE_MASTER_PRIVATE_KEY_1,
                         WG_SERVER_PUBLIC_IP, WG_SERVER_PUBLIC_KEY, WG_ENDPOINT_PORT);
    if (!wgOk)
        Serial.println("WireGuard successfully initialized on ESP32!");
    else
        Serial.println("WireGuard initialization failed!");

    modbusClient = new ModbusClientTCPasync(modbusSlaveIP, modbusSlavePort);
    modbusClient->connect();
    modbusClient->onDataHandler(&onDataHandler);
    modbusClient->onErrorHandler(&onErrorHandler);
    modbusClient->setTimeout(10000);
    modbusClient->setIdleTimeout(60000);

    SPI.begin(TFT_SCK_PIN, TFT_MISO_PIN, TFT_MOSI_PIN, TFT_CS_PIN);
    display.begin();
    display.setHeaderTitle("BANDUNG-SEEDBED-1");
    display.setFooterText("v1.0.0");
    display.setContainer1("TEMPERATURE", "WAITING FOR DATA...", DisplayColor::ORANGE, IconType::THERMO);
    display.setContainer2("HUMIDITY", "WAITING FOR DATA...", DisplayColor::BLUE, IconType::DROPLET);
    display.setContainer3("WIND SPEED", "WAITING FOR DATA...", DisplayColor::TEXT, IconType::WIND);
    display.setContainer4("WIND DIR", "WAITING FOR DATA...", DisplayColor::YELLOW, IconType::COMPASS);
    display.setContainer5("RAINFALL", "WAITING FOR DATA...", DisplayColor::TEAL, IconType::RAIN);
    display.setContainer6("COMPANY", "T4T x ZTS", DisplayColor::GREEN, IconType::COMPANY);
    display.drawLayout(); // one full paint of shells/borders/icons/labels
}

void loop()
{
    ElegantOTA.loop();
    wifi.reconnect();

    if (millis() - prevSystemLoggingMillis > 60000 * 2) // Every 2 minute
    {
        Serial.println("=== Task Sending data to supabase running ===");
        WebSerial.println("=== Task Sending data to supabase running ===");
        prevSystemLoggingMillis = millis();

        SystemLogDto systemLog{
            .deviceId = deviceId,
            .freeHeap = ESP.getFreeHeap(),
            .largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
            .minFreeHeap = ESP.getMinFreeHeap(),
            .lastResetReason = esp_reset_reason(),
        };
        Serial.println(systemLog.toString());
        WebSerial.println(systemLog.toString());

        // wifi.setTransport(&transport);
        // char buffer[256];
        // systemLog.toJson(buffer, sizeof(buffer));
        // wifi.send("system_logs", buffer);
    }

    if (millis() - prevSamplingMillis > 10000) //  Every 10 second
    {
        Serial.println("=== Task sampling data running ===");
        WebSerial.println("=== Task sampling data running ===");
        prevSamplingMillis = millis();

        Error err = modbusClient->addRequest(
            (uint32_t)stampDataModbusCounter, // Token
            1, READ_HOLD_REGISTER, 0, 5);
        if (err != SUCCESS)
        {
            ModbusError e(err);
            Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
            WebSerial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
        }

        SensorDto sensor{
            .deviceId = deviceId,
            .rainFall = float(sensorDatas[2] / 10.0F),
            .windSpeed = float(sensorDatas[3] / 10.0F),
            .windDirection = static_cast<WindDirectionEnum>(sensorDatas[4]),
            .airTemperature = float(sensorDatas[0] / 10.0F),
            .airHumidity = float(sensorDatas[1] / 10.0F)};
        Serial.println(sensor.toString());
        WebSerial.println(sensor.toString());

        // wifi.setTransport(&transport);
        // char buffer[256];
        // sensor.toJson(buffer, sizeof(buffer));
        // wifi.send("sensors", buffer);

        char buf[24]; // Follow max char in custom library
        snprintf(buf, sizeof(buf), "%.1f C", sensor.airTemperature);
        display.updateContainerValue(1, buf);

        snprintf(buf, sizeof(buf), "%.1f %RH", sensor.airHumidity);
        display.updateContainerValue(2, buf);

        snprintf(buf, sizeof(buf), "%.1f km/h", sensor.windSpeed);
        display.updateContainerValue(3, buf);

        snprintf(buf, sizeof(buf), "%s", Parser::parseWindDirection(sensor.windDirection));
        display.updateContainerValue(4, buf);

        snprintf(buf, sizeof(buf), "%.1f mm/day", sensor.rainFall);
        display.updateContainerValue(5, buf);

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("[ERROR] NTP Error");
            WebSerial.println("[ERROR] NTP Error");
        }
        char timeHour[3];
        char timeMinute[3];
        char timeSecond[3];
        strftime(timeHour, 3, "%H", &timeinfo);
        strftime(timeMinute, 3, "%M", &timeinfo);
        strftime(timeSecond, 3, "%S", &timeinfo);
        display.setClock(atoi(timeHour), atoi(timeMinute), atoi(timeSecond));

        display.setSignalStrength(wifi.getRssi());
        display.refresh();
        stampDataModbusCounter++;
    }
}

void onDataHandler(ModbusMessage response, uint32_t token)
{
    errorTransactionModbusCounter = 0; // Reset error counter since transaction work again

    uint16_t offset = 3;
    offset = response.get(offset, sensorDatas[0]);
    offset = response.get(offset, sensorDatas[1]);
    offset = response.get(offset, sensorDatas[2]);
    offset = response.get(offset, sensorDatas[3]);
    offset = response.get(offset, sensorDatas[4]);
    Serial.printf("[INFO] Success Parse: [%u, %u, %u, %u, %u]\n",
                  sensorDatas[0], sensorDatas[1], sensorDatas[2], sensorDatas[3], sensorDatas[4]);
}

void onErrorHandler(Error error, uint32_t token)
{
    Serial.printf("[ERROR] token: %d | code: %d\n", token, error);
    WebSerial.printf("[ERROR] token: %d | code: %d\n", token, error);

    if (errorTransactionModbusCounter > 10)
        ESP.restart();
    errorTransactionModbusCounter++;
}