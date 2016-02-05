#pragma once

#include "config.h"

#include <mutex>
#include <memory>
#include <thread>
#include <map>
#include <queue>
#include <functional>

#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>

#include "lsignal/lsignal.h"

#include "istream.h"
#include "ostream.h"
#include "socket.h"

namespace lnetlib
{
class connection
    : public std::enable_shared_from_this<connection>
{
public:
    enum custom_error
    {
        no_such_file_or_directory = 2,	// ENOENT
        ssl_short_read = 335544539		// SSL_R_SHORT_READ
    };

    using error_code = boost::system::error_code;
    using tcp = boost::asio::ip::tcp;
    using data_buffer = boost::asio::const_buffers_1;
    using stream_buffer = boost::asio::streambuf;

    using package_buffer = std::vector<stream_buffer::const_buffers_type>;
    using callback = std::function<void(std::unique_ptr<istream>)>;
    using dispatch_slot = std::function<void(std::unique_ptr<stream_buffer> buffer)>;

    lsignal::signal<void(std::shared_ptr<connection>)> closed;
    lsignal::signal<void(std::shared_ptr<connection>, int, const std::string&)> error;
    lsignal::signal<void(std::shared_ptr<connection>, std::unique_ptr<istream>)> received;

    connection(std::shared_ptr<socket> sckt);
    ~connection();

    bool is_open() const;
    void close();

    template<typename T>
    std::unique_ptr<ostream> create_stream(T command);

    template<typename T>
    std::unique_ptr<ostream> create_stream(T command, callback cb);

private:
    struct package
    {
        std::unique_ptr<package_buffer> buffer;
        std::unique_ptr<stream_buffer> size_buffer;
        std::unique_ptr<stream_buffer> data_buffer;
    };

    std::mutex _mutex;
    std::shared_ptr<socket> _socket;
    bool _socket_locked;
    std::queue<std::shared_ptr<package>> _packages;
    uint64_t _uid_counter;
    std::map<uint64_t, callback> _callbacks;

    bool check_error(const error_code& err);

    void wait_package();
    void wait_package_body(uint64_t size);

    void read_package_size_handler(std::shared_ptr<stream_buffer> buffer, const error_code& err, std::size_t bytes);
    void read_package_body_handler(std::shared_ptr<stream_buffer> buffer, const error_code& err, std::size_t bytes);

    std::shared_ptr<package> create_package(std::unique_ptr<stream_buffer> buffer) const;
    void send_package(std::shared_ptr<package> pkg);

    dispatch_slot create_dispatch_slot();

};

template<typename T>
std::unique_ptr<ostream> connection::create_stream(T command)
{
    std::unique_ptr<ostream> stream{ new ostream(command, _uid_counter++) };

    stream->dispatch.connect(create_dispatch_slot());

    return stream;
}

template<typename T>
std::unique_ptr<ostream> connection::create_stream(T command, callback cb)
{
    std::unique_ptr<ostream> stream = create_stream(command);

    _callbacks[stream->uid()] = cb;

    return stream;
}
}
