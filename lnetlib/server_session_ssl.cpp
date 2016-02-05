#include "server_session_ssl.h"

namespace lnetlib
{
server_session_ssl::server_session_ssl(service& srv, std::shared_ptr<tcp::acceptor> acceptor, std::shared_ptr<encryption> encr)
    : _encryption(encr), _socket(new ssl_socket(srv, encr->context())), _acceptor(acceptor)
{
}

server_session_ssl::~server_session_ssl()
{
}

void server_session_ssl::start()
{
    _socket->set_verify_mode(_encryption->verify_mode());

    auto verify_callback = [this](bool preverified, verify_context& context)
    {
        return _encryption->verify_certificate(preverified, context);
    };

    _socket->set_verify_callback(verify_callback);

    auto handler = std::bind(&server_session_ssl::accept_handler, this, std::placeholders::_1);

    _acceptor->async_accept(_socket->lowest_layer(), handler);
}

void server_session_ssl::accept_handler(const error_code& err)
{
    if (err)
    {
        connected(nullptr, err);

        return;
    }

    auto handler = std::bind(&server_session_ssl::handshake_handler, this, std::placeholders::_1);

    _socket->async_handshake(ssl_stream_base::server, handler);
}

void server_session_ssl::handshake_handler(const error_code& err)
{
    std::shared_ptr<connection> conn;

    if (!err)
    {
        std::shared_ptr<socket> sckt = std::make_shared<socket_ssl>(_socket);

        conn = std::make_shared<connection>(sckt);
    }

    connected(conn, err);
}
}
