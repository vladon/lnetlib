#pragma once

#ifdef _WIN32
#include <SDKDDKVer.h>
#include <tchar.h>
#endif

// C Lib
#include <csignal>
#include <cstdint>
#include <cstdio>

// STL
#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// Boost
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/verify_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
