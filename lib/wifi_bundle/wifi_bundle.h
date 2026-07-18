#ifndef WIFI_BUNDLE_H
#define WIFI_BUNDLE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>

#include "transport.h"

class WiFiBundle
{
public:
    WiFiBundle(const char *ssid, const char *password, const char *hostname);
    ~WiFiBundle();

    bool begin(bool restartOnFail = true);
    const char *localIP();
    void reconnect();

    void setTransport(DataTransport *transport)
    {
        delete _transport;
        _transport = transport;
    }
    DataTransport *getTransport() const { return _transport; }

    int post(const char *url, const char *payload);

private:
    const char *_ssid;
    const char *_password;
    const char *_hostname;
    char _localIP[16];
    DataTransport *_transport = nullptr;
    const uint8_t _maxRetry = 3;

    uint8_t _counterReset = 0;

    bool _setupMDNS()
    {
        if (!MDNS.begin(_hostname))
            return false;
        MDNS.addService("http", "tcp", 80);
        return true;
    }
};

#endif