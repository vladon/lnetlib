#include "socket_ssl.h"

namespace lnetlib
{
socket_ssl::socket_ssl(std::shared_ptr<ssl_socket> socket)
    : _socket(socket)
{
}

socket_ssl::~socket_ssl()
{
}

bool socket_ssl::is_open() const
{
    return _socket->lowest_layer().is_open();
}

void socket_ssl::close()
{
    _socket->lowest_layer().shutdown(socket_base::shutdown_receive);
    _socket->lowest_layer().close();
}

void socket_ssl::async_read(stream_buffer& buffer, completion_cond cond, async_read_handler handler)
{
    boost::asio::async_read(*_socket, buffer, cond, handler);
}

void socket_ssl::async_write(const package_buffer& buffer, async_write_handler handler)
{
    boost::asio::async_write(*_socket, buffer, handler);
}
}
