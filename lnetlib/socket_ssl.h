#pragma once

#include "config.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "socket.h"

namespace lnetlib
{
class socket_ssl
    : public socket
{
public:
    using tcp = boost::asio::ip::tcp;
    using socket_base = boost::asio::socket_base;
    using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

    socket_ssl(std::shared_ptr<ssl_socket> socket);
    ~socket_ssl() override;

    bool is_open() const override;
    void close() override;

    void async_read(stream_buffer& buffer, completion_cond cond, async_read_handler handler) override;
    void async_write(const package_buffer& buffer, async_write_handler handler) override;

private:
    std::shared_ptr<ssl_socket> _socket;

};
}
