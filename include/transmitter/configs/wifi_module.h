#if !defined(WIFI_MODULE_H)
#define WIFI_MODULE_H

#include <WiFi.h>
#include <HTTPClient.h>

#include "wifi_bundle.h"
#include "../base_transmitter.h"

/**
 * @brief WiFi Module Class
 *
 * @param ssid const char* (WiFi SSID)
 * @param password const char* (WiFi Password)
 * @param hostname const char* (Hostname for the Device)
 *
 */
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
            return true;
        return false;
    }

    IPAddress *resolveMDNS(const char *hostname)
    {
        static IPAddress query = MDNS.queryHost(hostname);
        if (query.toString() == "0.0.0.0")
            return nullptr;
        return &query;
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
            _inet.setTransport(supabaseConfig);
        _inet.post(url, payload);
    }
};

#endif // WIFI_MODULE_H
