#pragma once

#include "config.h"

#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/connect.hpp>

#include "lsignal/lsignal.h"

#include "connection.h"

namespace lnetlib
{
class client_session
{
public:
    using error_code = boost::system::error_code;
    using service = boost::asio::io_service;
    using tcp = boost::asio::ip::tcp;

    lsignal::signal<void(std::shared_ptr<connection>, const error_code&)> connected;

    client_session();
    virtual ~client_session();

    virtual void start() = 0;

};
}