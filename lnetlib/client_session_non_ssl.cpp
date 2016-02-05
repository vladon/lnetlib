#include "client_session_non_ssl.h"

namespace lnetlib
{
client_session_non_ssl::client_session_non_ssl(service& srv, tcp::resolver::iterator endpoint)
    : _socket(new tcp::socket(srv)), _endpoint(endpoint)
{
}

client_session_non_ssl::~client_session_non_ssl()
{
}

void client_session_non_ssl::start()
{
    auto handler = std::bind(&client_session_non_ssl::connect_handler, this, std::placeholders::_1);

    async_connect(*_socket, _endpoint, handler);
}

void client_session_non_ssl::connect_handler(const error_code& err)
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
