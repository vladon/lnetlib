#pragma once

#include "config.h"

#include <functional>

#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>

namespace lnetlib
{
	class socket
	{
	public:
		using error_code = boost::system::error_code;
		using stream_buffer = boost::asio::streambuf;
		using package_buffer = std::vector<stream_buffer::const_buffers_type>;
		using completion_cond = std::function<std::size_t(const error_code&, std::size_t)>;
		using async_read_handler = std::function<void(const error_code&, std::size_t)>;
		using async_write_handler = std::function<void(const error_code&, std::size_t)>;

		socket();
		virtual ~socket();

		virtual bool is_open() const = 0;
		virtual void close() = 0;

		virtual void async_read(stream_buffer& buffer, completion_cond cond, async_read_handler handler) = 0;
		virtual void async_write(const package_buffer& buffer, async_write_handler handler) = 0;

	};
}
