#include "client_session_ssl.h"

namespace lnetlib
{
client_session_ssl::client_session_ssl(service& srv, tcp::resolver::iterator endpoint, std::shared_ptr<encryption> encr)
    : _encryption(encr), _socket(new ssl_socket(srv, encr->context())), _endpoint(endpoint)
{
}

client_session_ssl::~client_session_ssl()
{
}

void client_session_ssl::start()
{
    _socket->set_verify_mode(_encryption->verify_mode());

    auto verify_callback = [this](bool preverified, verify_context& context)
    {
        return _encryption->verify_certificate(preverified, context);
    };

    _socket->set_verify_callback(verify_callback);

    auto handler = std::bind(&client_session_ssl::connect_handler, this, std::placeholders::_1);

    async_connect(_socket->lowest_layer(), _endpoint, handler);
}

void client_session_ssl::connect_handler(const error_code& err)
{
    if (err)
    {
        connected(nullptr, err);

        return;
    }

    auto handler = std::bind(&client_session_ssl::handshake_handler, this, std::placeholders::_1);

    _socket->async_handshake(ssl_stream_base::client, handler);
}

void client_session_ssl::handshake_handler(const error_code& err)
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
