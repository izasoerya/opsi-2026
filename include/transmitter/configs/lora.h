#if !defined(LORA_H)
#define LORA_H

#include <Stream.h>
#include "../base_transmitter.h"

class Lora : public BaseTransmitter
{
private:
    Stream *_stream;

public:
    Lora(Stream *stream) : _stream(stream) {}
    ~Lora() override = default;

    void send(const char *payload)
    {
        _stream->printf(payload);
    }
};

#endif // LORA_H
