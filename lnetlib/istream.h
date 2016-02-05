#pragma once

#include "config.h"

#include <istream>
#include <memory>

#include <boost/asio/streambuf.hpp>

#include "ostream.h"

namespace lnetlib
{
class istream
    : public std::istream
{
public:
    using stream_buffer = boost::asio::streambuf;

    lsignal::signal<void(std::unique_ptr<stream_buffer>)> response_dispatch;

    istream(std::shared_ptr<stream_buffer> buffer);
    ~istream();

    template<typename T>
    T command() const;

    uint64_t uid() const;

    int8_t read_int8();
    uint8_t read_uint8();

    int16_t read_int16();
    uint16_t read_uint16();

    int32_t read_int32();
    uint32_t read_uint32();

    int64_t read_int64();
    uint64_t read_uint64();

    float read_float32();
    double read_float64();

    template<typename T = uint64_t>
    std::string read_string();

    template<typename T = char, typename U = uint64_t>
    const T* read_data_chunk(U& size);

    template<typename T = char, typename U = uint64_t>
    std::vector<T> read_data_chunk();

    std::unique_ptr<ostream> create_response();

private:
    std::shared_ptr<stream_buffer> _buffer;
    uint64_t _uid;
    uint64_t _command;

    template<typename T>
    T read_basic();

};

template<typename T>
T istream::command() const
{
    return static_cast<T>(_command);
}

template<typename T>
std::string istream::read_string()
{
    T size = read_basic<T>();

    char data[size];
    read(data, size);

    return std::string(data, 0, size);
}

template<typename T, typename U>
const T* istream::read_data_chunk(U& size)
{
    size = read_basic<U>();

    T *data = new char[size];
    read(reinterpret_cast<char*>(data), size * sizeof(T));

    return data;
}

template<typename T, typename U>
std::vector<T> istream::read_data_chunk()
{
    U size = read_basic<U>();

    std::vector<T> chunk(size);
    read(reinterpret_cast<char*>(chunk.data()), chunk.size() * sizeof(T));

    return chunk;
}

template<typename T>
T istream::read_basic()
{
    T data;

    read(reinterpret_cast<char*>(&data), sizeof(T));

    return data;
}
}
