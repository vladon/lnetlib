#pragma once

#include "config.h"

#include "client_session.h"
#include "socket_non_ssl.h"

namespace lnetlib
{
class client_session_non_ssl
    : public client_session
{
public:
    client_session_non_ssl(service& srv, boost::asio::ip::tcp::resolver::iterator endpoint);
    ~client_session_non_ssl() override;

    void start() override;

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    boost::asio::ip::tcp::resolver::iterator _endpoint;

    void connect_handler(const error_code& err);

};

}
