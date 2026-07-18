#include <Arduino.h>
#include <HTTPClient.h>

class DataTransport
{
public:
    virtual ~DataTransport() = default;

    virtual int post(const char *endpoint, const char *payload,
                     const char *contentType = "application/json") = 0;
};

class SupabaseTransport : public DataTransport
{
private:
    const char *_supabaseUrl;
    const char *_anonKey;

    void _setHeaders(HTTPClient &http, const String &contentType)
    {
        http.addHeader("Content-Type", contentType);
        http.addHeader("apikey", _anonKey);
        http.addHeader("Authorization", String("Bearer ") + _anonKey);
        http.addHeader("Prefer", "return=minimal");
    }

public:
    SupabaseTransport(const char *supabaseUrl, const char *anonKey);
    ~SupabaseTransport();

    int post(const char *tableName,
             const char *payload,
             const char *contentType = "application/json") override;
};

class BasicHTTPTransport : public DataTransport
{
public:
    BasicHTTPTransport();
    ~BasicHTTPTransport();

    int post(const char *endpoint,
             const char *payload,
             const char *contentType = "application/json") override;
};