#pragma once

#include "config.h"

#include <memory>
#include <string>
#include <thread>

#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/ssl/context.hpp>

#include "lsignal/lsignal.h"

#include "helper.h"
#include "istream.h"
#include "ostream.h"

#include "encryption.h"
#include "connection.h"

#include "client_session.h"
#include "client_session_ssl.h"
#include "client_session_non_ssl.h"

namespace lnetlib
{
class client
{
public:
    using error_code = boost::system::error_code;
    using service = boost::asio::io_service;
    using tcp = boost::asio::ip::tcp;
    using socket_base = boost::asio::socket_base;
    using address = boost::asio::ip::address;

    lsignal::signal<void()> refused;
    lsignal::signal<void()> connected;
    lsignal::signal<void()> disconnected;
    lsignal::signal<void(int, const std::string&)> error;
    lsignal::signal<void(std::unique_ptr<istream> stream)> received;

    client();
    ~client();

    std::shared_ptr<encryption> encryption_info() const;

    void connect(const std::string& hostname, int port);
    void disconnect();

    std::shared_ptr<connection> conn() const;

private:
    bool _running;
    service _service;
    tcp::resolver::iterator _endpoint;

    std::shared_ptr<encryption> _encryption;
    std::shared_ptr<connection> _connection;
    std::shared_ptr<client_session> _session;

    void try_connect();

    void create_session();

    void connected_handler(client_session *session, std::shared_ptr<connection> conn, const error_code& err);
    void error_handler(std::shared_ptr<connection> conn, int code, const std::string& message);
    void received_handler(std::shared_ptr<connection> conn, std::unique_ptr<istream> stream);

};
}
