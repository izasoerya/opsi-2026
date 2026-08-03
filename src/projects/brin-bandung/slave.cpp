#include <Arduino.h>
#include <ModbusServerTCPasync.h>
#include <ElegantOTA.h>
#include <WebSerial.h>
#include <Wire.h>
#include <SHT2x.h>

#include "transmitter/configs/wifi_module.h"
#include "../utils/utils.h"
#include "models.h"

const char *ssid = "NodeSensorWiFi1";
const char *password = "muhammadnabiyullah";
const char *hostname = "slave-bandung-persemaian-1";
WiFiModule wifi(ssid, password, hostname);

AsyncWebServer server(80);

ModbusServerTCPasync modbusServer;
const uint8_t MAX_REGISTER = 16;
uint16_t modbusData[MAX_REGISTER];
ModbusMessage FC03(ModbusMessage request);

const uint8_t pinAnemo = 2;           // TODO: CHANGE TO APPROPRIATE PIN
const uint8_t pinRainfall = 3;        // TODO: CHANGE TO APPROPRIATE PIN
const uint8_t pinWindDirectionRX = 4; // TODO: CHANGE TO APPROPRIATE PIN
const uint8_t pinWindDirectionTX = 5; // TODO: CHANGE TO APPROPRIATE PIN
const uint8_t pinSDA = 4;             // TODO: CHANGE TO APPROPRIATE PIN
const uint8_t pinSCL = 5;             // TODO: CHANGE TO APPROPRIATE PIN

SHT2x sht;
volatile uint32_t counterAnemo = 0;
volatile uint32_t counterRainfall = 0;
uint32_t prevTimeReading = 0;
uint32_t delayReading = 10000;

void setup()
{
    Serial.begin(115200);
    // Serial1.begin(9600, SERIAL_8N1, pinWindDirectionTX, pinWindDirectionRX);

    if (wifi.begin())
        Serial.println(wifi.localIP());

    ElegantOTA.setAutoReboot(true);
    ElegantOTA.begin(&server);
    WebSerial.begin(&server);
    server.begin();

    Wire.begin(pinSDA, pinSCL);
    if (!sht.begin())
    {
        Serial.println("SHTX is not working");
        WebSerial.println("SHTX is not working");
    }

    attachInterrupt(
        pinAnemo, []() -> void ARDUINO_ISR_ATTR
        { counterAnemo++; }, RISING);
    attachInterrupt(
        pinRainfall, []() -> void ARDUINO_ISR_ATTR
        { counterRainfall++; }, RISING);

    modbusServer.registerWorker(1, READ_HOLD_REGISTER, &FC03); // FC=03 for serverID=1
    modbusServer.start(5000, 2, 20000);
}

void loop()
{
    if (millis() - prevTimeReading > delayReading)
    {
        // if (Serial1.available())
        // {
        //     String data = Serial1.readString(); // data yang diterima dari sensor berawalan tanda * dan diakhiri tanda #, contoh *1#
        //     int a = data.indexOf("*");          // a adalah index tanda *
        //     int b = data.indexOf("#");          // b adalah index tanda #
        //     String resultWind = data.substring(a + 1, b);
        //     modbusData[4] = Parser::parseStringWindDirection(resultWind);
        // }

        // modbusData[0] = sht.getTemperature();
        // modbusData[1] = sht.getHumidity();
        // modbusData[2] = counterRainfall;
        // modbusData[3] = counterAnemo;

        // === SENSOR DATA ===
        modbusData[0] = random(32768);
        modbusData[1] = random(32768);
        modbusData[2] = random(32768);
        modbusData[3] = random(32768);
        modbusData[4] = random(8);

        // === SYSTEM LOG DATA ===
        uint32_t freeHeap = ESP.getFreeHeap();
        modbusData[5] = (uint16_t)(freeHeap >> 16);    // High word
        modbusData[6] = (uint16_t)(freeHeap & 0xFFFF); // Low word
        uint32_t largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
        modbusData[7] = (uint16_t)(largestBlock >> 16);
        modbusData[8] = (uint16_t)(largestBlock & 0xFFFF);
        uint32_t minHeap = ESP.getMinFreeHeap();
        modbusData[9] = (uint16_t)(minHeap >> 16);
        modbusData[10] = (uint16_t)(minHeap & 0xFFFF);
        modbusData[11] = (uint16_t)esp_reset_reason();

        // TODO: IMPLEMENT RESET ISR COUNTER EVERY 12 PM
        // TODO: (OPTIONAL) STORE COUNTER AT EEPROM IN CASE OF WATCHDOG / RESET
    }
}

ModbusMessage FC03(ModbusMessage request)
{
    /**
     * @brief Info about modbus TCP frame
     * | Slave id | Function code | Start Add | Length Add |
     * | 1 bytes  | 1 bytes       | 2 bytes   | 2 bytes    |
     */
    ModbusMessage response; // in
    uint16_t addr = 0;      // start address
    uint16_t words = 0;     // # of words requested
    request.get(2, addr);   // since start address is on bytes 3 of modbus frame
    request.get(4, words);  // since length address is on bytes 5 of modbus frame

    if ((addr + words) > MAX_REGISTER)
        response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);

    response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));
    if (request.getFunctionCode() == READ_HOLD_REGISTER)
    {
        for (uint8_t i = 0; i < words; i++)
            response.add((uint16_t)modbusData[addr + i]);

        Serial.printf("Req Slave Id: %d, FC: %d, Data: [%d, %d, %d, %d, %d]",
                      request.getServerID(), request.getFunctionCode(),
                      modbusData[addr + 0], modbusData[addr + 1], modbusData[addr + 2], modbusData[addr + 3], modbusData[addr + 4]);

        WebSerial.printf("Req Slave Id: %d, FC: %d, Data: [%d, %d, %d, %d, %d]",
                         request.getServerID(), request.getFunctionCode(),
                         modbusData[addr + 0], modbusData[addr + 1], modbusData[addr + 2], modbusData[addr + 3], modbusData[addr + 4]);
    }
    return response;
}