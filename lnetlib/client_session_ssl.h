#pragma once

#include "config.h"

#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/verify_context.hpp>

#include "client_session.h"
#include "encryption.h"
#include "socket_ssl.h"

namespace lnetlib
{
class client_session_ssl
    : public client_session
{
public:
    using ssl_socket = boost::asio::ssl::stream<tcp::socket>;
    using ssl_stream_base = boost::asio::ssl::stream_base;
    using verify_context = boost::asio::ssl::verify_context;

    client_session_ssl(service& srv, boost::asio::ip::tcp::resolver::iterator endpoint, std::shared_ptr<encryption> encr);
    ~client_session_ssl() override;

    void start() override;

private:
    std::shared_ptr<encryption> _encryption;
    std::shared_ptr<ssl_socket> _socket;
    boost::asio::ip::tcp::resolver::iterator _endpoint;

    void connect_handler(const error_code& err);
    void handshake_handler(const error_code& err);

};

}
