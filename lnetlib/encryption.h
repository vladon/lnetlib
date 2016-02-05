#pragma once

#include "config.h"

#include <functional>

#include <boost/asio/ssl/context.hpp>

#include "helper.h"

namespace lnetlib
{
enum class encryption_method
{
    sslv2 = boost::asio::ssl::context::sslv2,
    sslv3 = boost::asio::ssl::context::sslv3,
    tlsv1 = boost::asio::ssl::context::tlsv1,
    sslv23 = boost::asio::ssl::context::sslv23,
    tlsv11 = boost::asio::ssl::context::tlsv11,
    tlsv12 = boost::asio::ssl::context::tlsv12
};

enum ssl_verify_mode
{
    ssl_verify_none = boost::asio::ssl::verify_none,
    ssl_verify_peer = boost::asio::ssl::verify_peer,
    ssl_verify_fail_if_no_peer_cert = boost::asio::ssl::verify_fail_if_no_peer_cert,
    ssl_verify_client_once = boost::asio::ssl::verify_client_once
};

class encryption
{
public:
    using const_buffer = boost::asio::const_buffer;
    using ssl_context = boost::asio::ssl::context;
    using verify_context = boost::asio::ssl::verify_context;

    using verify_callback = std::function<bool(bool, X509_STORE_CTX*)>;

    encryption();
    ~encryption();

    void init(encryption_method method);

    bool is_enabled() const;
    void set_enabled(bool enabled);

    std::string ca_filename() const;
    void set_ca_filename(const std::string& filename);

    std::string cert_filename() const;
    void set_cert_filename(const std::string& filename);

    std::string private_key_filename() const;
    void set_private_key_filename(const std::string& filename);

    int verify_mode() const;
    void set_verify_mode(int mode);

    void set_verify_callback(verify_callback callback);
    bool verify_certificate(bool preverified, verify_context& context);

    ssl_context& context();

    std::string last_error() const;

private:
    bool _enabled;
    encryption_method _method;
    std::string _ca_filename;
    std::string _cert_filename;
    std::string _private_key_filename;
    int _verify_mode;
    verify_callback _verify_callback;
    ssl_context _context;
    std::string _last_error;

};
}
