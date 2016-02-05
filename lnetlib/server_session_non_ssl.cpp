#include "server_session_non_ssl.h"

namespace lnetlib
{
server_session_non_ssl::server_session_non_ssl(service& srv, std::shared_ptr<tcp::acceptor> acceptor)
    : _socket(new tcp::socket(srv)), _acceptor(acceptor)
{
}

server_session_non_ssl::~server_session_non_ssl()
{
}

void server_session_non_ssl::start()
{
    auto handler = std::bind(&server_session_non_ssl::accept_handler, this, std::placeholders::_1);

    _acceptor->async_accept(*_socket.get(), handler);
}

void server_session_non_ssl::accept_handler(const error_code& err)
{
    std::shared_ptr<connection> conn;

    if (!err)
    {
        std::shared_ptr<socket> sckt = std::make_shared<socket_non_ssl>(_socket);

        conn = std::make_shared<connection>(sckt);
    }

    connected(conn, err);
}
}
