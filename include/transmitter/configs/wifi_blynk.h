#if !defined(WIFI_BLYNK)
#define WIFI_BLYNK

#include <BlynkSimpleEsp32.h>

class WiFiBlynk;

inline WiFiBlynk *&wifiBlynkInstance()
{
    static WiFiBlynk *instance = nullptr;
    return instance;
}

class WiFiBlynk
{
private:
    const char *_authToken;
    const char *_ssid;
    const char *_password;
    std::function<void(int, bool)> _callback;

public:
    WiFiBlynk(const char *authToken, const char *ssid, const char *password, std::function<void(int, bool)> callback)
        : _authToken(authToken), _ssid(ssid), _password(password), _callback(callback)
    {
        wifiBlynkInstance() = this;
    }
    ~WiFiBlynk() {}

    void begin()
    {
        Blynk.begin(_authToken, _ssid, _password);
    }

    void send(uint8_t virtualPin, float value)
    {
        Blynk.virtualWrite(virtualPin, value);
    }

    void run()
    {
        Blynk.run();
    }

    void onCallback(uint8_t pin, bool value)
    {
        _callback(pin, value);
    }
};

BLYNK_WRITE_DEFAULT()
{
    if (wifiBlynkInstance() != nullptr)
    {
        wifiBlynkInstance()->onCallback(request.pin, param.asInt());
    }
}

#endif // WIFI_BLYNK
