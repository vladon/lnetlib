#include "istream.h"

namespace lnetlib
{
istream::istream(std::shared_ptr<stream_buffer> buffer)
    : _buffer(buffer)
{
    init(_buffer.get());

    this->read(reinterpret_cast<char*>(&_command), sizeof(uint64_t));
    this->read(reinterpret_cast<char*>(&_uid), sizeof(uint64_t));
}

istream::~istream()
{
}

uint64_t istream::uid() const
{
    return _uid;
}

int8_t istream::read_int8()
{
    return read_basic<int8_t>();
}

uint8_t istream::read_uint8()
{
    return read_basic<uint8_t>();
}

int16_t istream::read_int16()
{
    return read_basic<int16_t>();
}

uint16_t istream::read_uint16()
{
    return read_basic<uint16_t>();
}

int32_t istream::read_int32()
{
    return read_basic<int32_t>();
}

uint32_t istream::read_uint32()
{
    return read_basic<uint32_t>();
}

int64_t istream::read_int64()
{
    return read_basic<int64_t>();
}

uint64_t istream::read_uint64()
{
    return read_basic<uint64_t>();
}

float istream::read_float32()
{
    return read_basic<float>();
}

double istream::read_float64()
{
    return read_basic<double>();
}

std::unique_ptr<ostream> istream::create_response()
{
    std::unique_ptr<ostream> stream{ new ostream(_command, _uid) };

    stream->dispatch.connect([this](std::unique_ptr<stream_buffer> buffer)
    {
        response_dispatch(std::move(buffer));
    });

    return stream;
}
}
