#include "socket_non_ssl.h"

namespace lnetlib
{
socket_non_ssl::socket_non_ssl(std::shared_ptr<tcp::socket> socket)
    : _socket(socket)
{
}

socket_non_ssl::~socket_non_ssl()
{
}

bool socket_non_ssl::is_open() const
{
    return _socket->is_open();
}

void socket_non_ssl::close()
{
    _socket->shutdown(socket_base::shutdown_both);
    _socket->close();
}

void socket_non_ssl::async_read(stream_buffer& buffer, completion_cond cond, async_read_handler handler)
{
    boost::asio::async_read(*_socket, buffer, cond, handler);
}

void socket_non_ssl::async_write(const package_buffer& buffer, async_write_handler handler)
{
    boost::asio::async_write(*_socket, buffer, handler);
}
}
