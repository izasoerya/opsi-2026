#if !defined(MOVING_AVERAGE)
#define MOVING_AVERAGE

#include "base_filter.h"

class MovingAverageFilter : public BaseFilter
{
private:
    float *_buffer;
    uint8_t _windowSize;
    uint8_t _index = 0;

public:
    MovingAverageFilter(uint8_t windowSize = 10) : _windowSize(windowSize), _buffer(new float[_windowSize])
    {
        for (int i = 0; i < _windowSize; i++)
            _buffer[i] = 0;
    }
    ~MovingAverageFilter() override { delete[] _buffer; }

    void filter(float &raw) override
    {
        _buffer[_index] = raw;
        _index = (_index + 1) % _windowSize;

        float sum = 0.0f;
        for (int i = 0; i < _windowSize; i++)
        {
            sum += _buffer[i];
        }
        raw = sum / _windowSize;
    }
};

#endif // MOVING_AVERAGE
