#include "wifi_bundle.h"

WiFiBundle::WiFiBundle(const char *ssid, const char *password, const char *hostname)
    : _ssid(ssid),
      _password(password),
      _hostname(hostname) {}

WiFiBundle::~WiFiBundle()
{
    delete _transport;
}

bool WiFiBundle::begin(bool restartOnFail)
{
    WiFi.mode(WIFI_STA);
    WiFi.hostname(_hostname);
    WiFi.begin(_ssid, _password);

    uint8_t counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        counter++;
        if (counter > 40) // 20 second
            if (restartOnFail)
                ESP.restart();
            else
                return false;
    }
    if (!_setupMDNS())
        if (restartOnFail)
            ESP.restart();
        else
            return false;

    return true;
}

const char *WiFiBundle::localIP()
{
    IPAddress ip = WiFi.localIP();
    snprintf(_localIP, sizeof(_localIP), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return _localIP;
}

void WiFiBundle::reconnect()
{
    const uint32_t currentTime = millis();
    while (millis() < currentTime + 5000)
        if (WiFi.status() == WL_CONNECTED)
            return;

    WiFi.disconnect(true); // force clean state
    delay(100);
    begin();
}

int WiFiBundle::post(const char *url, const char *payload)
{
    int res;
    for (int i = 0; i < _maxRetry; i++)
    {
        int res = _transport->post(url, payload);
        if (res == 200 || res == 201)
            return res;
        delay(50);
    }
    return res;
}