#pragma once

#include "config.h"

#include <functional>

#include "server_session.h"
#include "socket_non_ssl.h"

namespace lnetlib
{
	class server_session_non_ssl
		: public server_session
	{
	public:
		using error_code = boost::system::error_code;
		using service = boost::asio::io_service;
		using tcp = boost::asio::ip::tcp;

		server_session_non_ssl(service& srv, std::shared_ptr<tcp::acceptor> acceptor);
		~server_session_non_ssl() override;

		void start() override;

	private:
		std::shared_ptr<tcp::socket> _socket;
		std::shared_ptr<tcp::acceptor> _acceptor;

		void accept_handler(const error_code& err);

	};
}
