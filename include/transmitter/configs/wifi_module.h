#if !defined(WIFI_MODULE)
#define WIFI_MODULE

#include <WiFi.h>
#include <HTTPClient.h>

#include "wifi_bundle.h"
#include "../base_transmitter.h"

class WiFiModule : public BaseTransmitter
{
private:
    WiFiBundle _inet;
    BasicHTTPTransport *basicConfig = nullptr;
    SupabaseTransport *supabaseConfig = nullptr;

public:
    WiFiModule(const char *ssid, const char *password, const char *hostname)
        : _inet(WiFiBundle(ssid, password, hostname)) {}
    ~WiFiModule() override = default;

    const char *localIP() { return _inet.localIP(); }

    void setTransport(BasicHTTPTransport *basicTransport) { basicConfig = basicTransport; }

    void setTransport(SupabaseTransport *supabaseTransport) { supabaseConfig = supabaseTransport; }

    bool begin()
    {
        if (_inet.begin(true)) // Restart on fail set to true
        {
            return true;
        }
        return false;
    }

    void reconnect()
    {
        _inet.reconnect();
    }

    void send(const char *url, const char *payload)
    {
        if (basicConfig != nullptr)
            _inet.setTransport(basicConfig);
        else if (supabaseConfig != nullptr)
            _inet.setTransport(basicConfig);
        _inet.post(url, payload);
    }
};

#endif // WIFI_MODULE
