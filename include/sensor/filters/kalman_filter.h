#include <cstring>
#if !defined(KALMAN_FILTER)
#define KALMAN_FILTER

class BaseFilter
{
public:
    virtual ~BaseFilter() = default;
    virtual void filter(float &raw) = 0; // Pass by reference to modify in-place
};

class KalmanFilter : public BaseFilter
{
private:
    const float _q; // Process noise
    const float _r; // Measurement noise
    float _x;       // State estimate
    float _p;       // Error covariance

public:
    KalmanFilter(float q, float r)
        : _q(q), _r(r), _x(0.0f), _p(1.0f) {}

    ~KalmanFilter() override = default;

    void filter(float &raw) override
    {
        // Predict
        _p = _p + _q;

        // Update
        float k = _p / (_p + _r); // Kalman gain
        _x = _x + k * (raw - _x);
        _p = (1.0f - k) * _p;

        // Return filtered value
        raw = _x;
    }
};

class MovingAverageFilter : public BaseFilter
{
private:
    static const unsigned char BUFFER_SIZE = 5;
    float _buffer[BUFFER_SIZE];
    int _index = 0;

public:
    MovingAverageFilter() { memset(_buffer, 0, sizeof(_buffer)); }
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

#endif // KALMAN_FILTER
