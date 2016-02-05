#pragma once

#include "config.h"

#include <boost/asio/ip/tcp.hpp>

#include "socket.h"

namespace lnetlib
{
class socket_non_ssl
    : public socket
{
public:
    using tcp = boost::asio::ip::tcp;
    using socket_base = boost::asio::socket_base;

    socket_non_ssl(std::shared_ptr<tcp::socket> socket);
    ~socket_non_ssl() override;

    bool is_open() const override;
    void close() override;

    void async_read(stream_buffer& buffer, completion_cond cond, async_read_handler handler) override;
    void async_write(const package_buffer& buffer, async_write_handler handler) override;

private:
    std::shared_ptr<tcp::socket> _socket;

};
}
