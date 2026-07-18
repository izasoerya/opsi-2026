#if !defined(DISPLAY_OLED_H)
#define DISPLAY_OLED_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayOLED
{
private:
    const uint8_t _width = 128;
    const uint8_t _height = 32;
    const int8_t _resetPin = -1;
    const uint8_t _address;
    Adafruit_SSD1306 _display;

public:
    /**
     * @brief Construct a new Display OLED object
     *
     * @param wire I2C object wire
     * @param address I2C address
     */
    DisplayOLED(TwoWire *wire, const uint8_t address)
        : _display(_width, _height, wire, _resetPin), _address(address) {}

    ~DisplayOLED() {}

    bool begin()
    {
        if (!_display.begin(SSD1306_SWITCHCAPVCC, _address))
            return false;
        return true;
    }

    /**
     * @brief Show message in OLED display
     *
     * @param data Capped at char[64], dont exceed it
     */
    void showMessage(const char *data)
    {
        static char buffer[64];
        snprintf(buffer, sizeof(buffer), "%s", data);

        _display.clearDisplay();
        _display.setTextSize(2); // Draw 2X-scale text
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(10, 0);
        _display.println(buffer);
        _display.display(); // Show initial text
    }
};

/**
 * @brief Construct a new Display OLED object
 *
 * @param wire I2C object wire
 * @param address I2C address
 */
class MockDisplayOLED
{
private:
    const uint8_t _width = 128;
    const uint8_t _height = 32;
    const int8_t _resetPin = -1;
    const uint8_t _address;
    Adafruit_SSD1306 _display;

public:
    MockDisplayOLED(TwoWire *wire, const uint8_t address)
        : _display(_width, _height, wire, _resetPin), _address(address) {}

    ~MockDisplayOLED() {}

    bool begin()
    {
        return true;
    }

    /**
     * @brief Show message in OLED display
     *
     * @param data Capped at char[64], dont exceed it
     */
    void showMessage(const char *data)
    {
        return;
    }
};

#endif // DISPLAY_OLED_H
