#pragma once

#include "config.h"

#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/verify_context.hpp>

#include "server_session.h"
#include "encryption.h"
#include "socket_ssl.h"

namespace lnetlib
{
class server_session_ssl
    : public server_session
{
public:
    using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
    using ssl_stream_base = boost::asio::ssl::stream_base;
    using verify_context = boost::asio::ssl::verify_context;

    server_session_ssl(service& srv, std::shared_ptr<tcp::acceptor> acceptor, std::shared_ptr<encryption> encr);
    ~server_session_ssl() override;

    void start() override;

private:
    std::shared_ptr<encryption> _encryption;
    std::shared_ptr<ssl_socket> _socket;
    std::shared_ptr<tcp::acceptor> _acceptor;

    void accept_handler(const error_code& err);
    void handshake_handler(const error_code& err);

};
}
