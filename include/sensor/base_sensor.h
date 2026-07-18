#if !defined(BASE_SENSOR_H)
#define BASE_SENSOR_H

class BaseSensor
{
private:
    unsigned char _id;
    const char *_name;

public:
    BaseSensor(unsigned char id, const char *name) : _id(id), _name(name) {}
    virtual ~BaseSensor() = default;

    virtual float read() = 0;
};

#endif // BASE_SENSOR_H
