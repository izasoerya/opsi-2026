#include "transport.h"

SupabaseTransport::SupabaseTransport(const char *supabaseUrl, const char *anonKey)
    : _supabaseUrl(supabaseUrl), _anonKey(anonKey) {}

SupabaseTransport::~SupabaseTransport() {}

int SupabaseTransport::post(const char *endpoint,
                            const char *payload,
                            const char *contentType)
{
    WiFiClientSecure secureClient; // Use the secure variant for HTTPS
    secureClient.setInsecure();
    HTTPClient http;
    char url[128];
    snprintf(url, sizeof(url), "%s/rest/v1/%s", _supabaseUrl, endpoint);

    http.begin(secureClient, url);
    _setHeaders(http, contentType);
    int response = http.POST(payload);
    http.end();
    return response;
}

BasicHTTPTransport::BasicHTTPTransport() {}

BasicHTTPTransport::~BasicHTTPTransport() {}

int BasicHTTPTransport::post(const char *endpoint,
                             const char *payload,
                             const char *contentType)
{
    WiFiClientSecure secureClient; // Use the secure variant for HTTPS
    secureClient.setInsecure();

    HTTPClient http;
    http.begin(secureClient, endpoint);
    http.addHeader("Content-Type", contentType);
    int response = http.POST(payload);
    http.end();
    return response;
}