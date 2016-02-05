#include "ostream.h"

namespace lnetlib
{
ostream::~ostream()
{
    send();
}

uint64_t ostream::uid() const
{
    return _uid;
}

void ostream::write_int8(int8_t data)
{
    write_basic<int8_t>(data);
}

void ostream::write_uint8(uint8_t data)
{
    write_basic<uint8_t>(data);
}

void ostream::write_int16(int16_t data)
{
    write_basic<int16_t>(data);
}

void ostream::write_uint16(uint16_t data)
{
    write_basic<uint16_t>(data);
}

void ostream::write_int32(int32_t data)
{
    write_basic<int32_t>(data);
}

void ostream::write_uint32(uint32_t data)
{
    write_basic<uint32_t>(data);
}

void ostream::write_int64(int64_t data)
{
    write_basic<int64_t>(data);
}

void ostream::write_uint64(uint64_t data)
{
    write_basic<uint64_t>(data);
}

void ostream::write_float32(float data)
{
    write_basic<float>(data);
}

void ostream::write_float64(double data)
{
    write_basic<double>(data);
}

void ostream::send()
{
    if (_sended)
    {
        return;
    }

    _sended = true;

    dispatch(std::move(_buffer));
}
}
