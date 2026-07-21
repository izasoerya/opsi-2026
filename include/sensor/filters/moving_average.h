#if !defined(MOVING_AVERAGE)
#define MOVING_AVERAGE

#include "base_filter.h"

class MovingAverageFilter : public BaseFilter
{
private:
    static const unsigned char BUFFER_SIZE = 5;
    float _buffer[BUFFER_SIZE];
    int _index = 0;

public:
    MovingAverageFilter()
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
            _buffer[i] = 0;
    }
    ~MovingAverageFilter() override = default;

    void filter(float &raw) override
    {
        _buffer[_index] = raw;
        _index = (_index + 1) % BUFFER_SIZE;

        float sum = 0.0f;
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            sum += _buffer[i];
        }
        raw = sum / BUFFER_SIZE;
    }
};

#endif // MOVING_AVERAGE
