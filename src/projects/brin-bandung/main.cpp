#include <TFT_eSPI.h>
#include "display/display_tft_spi_lcd/display_tft.h"

#define TFT_SCK_PIN 8
#define TFT_MISO_PIN 20
#define TFT_MOSI_PIN 9
#define TFT_CS_PIN 5

TFT_eSPI tft = TFT_eSPI();
DisplayTFT320X240P display(tft);

float roomTemp = 24.5;
int roomHumid = 58;
float windSpeed = 12.3;
const char *windDir = "NE";
float rainfall = 0.0;
unsigned long clockSeconds = 52200;

void setup()
{
    SPI.begin(TFT_SCK_PIN, TFT_MISO_PIN, TFT_MOSI_PIN, TFT_CS_PIN);
    display.begin();

    display.setHeaderTitle("BANDUNG-PERSEMAIAN-1");
    display.setFooterText("v1.0.0");

    display.setContainer1("TEMPERATURE", "24.5 C", DisplayColor::ORANGE, IconType::THERMO);
    display.setContainer2("HUMIDITY", "58 %", DisplayColor::BLUE, IconType::DROPLET);
    display.setContainer3("WIND SPEED", "12.3 km/h", DisplayColor::TEXT, IconType::WIND);
    display.setContainer4("WIND DIR", "NE", DisplayColor::YELLOW, IconType::COMPASS);
    display.setContainer5("RAINFALL", "0.0 mm", DisplayColor::TEAL, IconType::RAIN);
    display.setContainer6("COMPANY", "T4T x ZTS", DisplayColor::GREEN, IconType::COMPANY);

    display.drawLayout(); // one full paint of shells/borders/icons/labels
}

void loop()
{
    roomTemp += 0.1;
    if (roomTemp > 30.0)
        roomTemp = 22.0;

    windSpeed += 0.3;
    if (windSpeed > 25.0)
        windSpeed = 5.0;

    clockSeconds += 2;

    char buf[12];
    dtostrf(roomTemp, 4, 1, buf);
    display.updateContainerValue(1, (String(buf) + " C").c_str());

    dtostrf(windSpeed, 4, 1, buf);
    display.updateContainerValue(3, (String(buf) + " km/h").c_str());

    unsigned long h = (clockSeconds / 3600) % 24;
    unsigned long m = (clockSeconds / 60) % 60;
    unsigned long s = clockSeconds % 60;
    display.setClock(h, m, s);
    display.setSignalStrength(-55);
    display.refresh(); // cheap: values + clock only, no shell repaint

    delay(2000);
}