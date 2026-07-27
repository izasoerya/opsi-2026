#include <TFT_eSPI.h>
#include <SensorBuilder.h>
#include <SoftwareSerial.h>
#include "time.h"

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
const char *hostName = "Bandung-Persemaian-1";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 3600;
WiFiModule wifi(ssid, password, hostName);

EspSoftwareSerial::UART sserial;
MockModbusRTUBuilder modbusNano(sserial);

void setup()
{
    delay(2000); // Wait NANO until its turned on

    Serial.begin(115200);
    sserial.begin(9600, SWSERIAL_8N1, 2, 3);

    if (wifi.begin())
        Serial.printf("Connected to: %s\n", wifi.localIP());
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    modbusNano.setSlaveId(1)
        .setFunctionCode(0x03)
        .setAddress(0x00)
        .setLengthAddress(5);
    ReadResult begin = modbusNano.connect();
    if (!begin.isOk())
        Serial.println(begin.errorMessage());

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
    ReadResult modbusResult[5];
    for (int i = 0; i < 5; i++)
    {
        modbusResult[i] = modbusNano.read(i);
        if (!modbusResult[i].isOk())
            Serial.println(modbusResult[i].errorMessage());
    }
    BandungPersemaianEntity sensor{
        .rainFall = random(100),       // float(modbusResult[0].value / 10.0F),
        .windSpeed = random(100),      // float(modbusResult[1].value / 10.0F),
        .windDirection = "NE",         // Parser::parseWindDirection(static_cast<WindDirectionEnum>(modbusResult[2].value)),
        .airTemperature = random(100), // float(modbusResult[3].value / 10.0F),
        .airHumidity = random(100),    // float(modbusResult[4].value / 10.0F)
    };

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

    delay(2000);
}