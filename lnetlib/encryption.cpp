#include "encryption.h"

namespace lnetlib
{
encryption::encryption()
    : _enabled(false), _method(encryption_method::tlsv11), _verify_mode(ssl_verify_none),
    _context(static_cast<ssl_context::method>(_method))
{
}

encryption::~encryption()
{
}

void encryption::init(encryption_method method)
{
    _last_error.clear();

    _method = method;

    _ca_filename.clear();
    _cert_filename.clear();
    _private_key_filename.clear();
    _verify_mode = ssl_verify_none;
    _verify_callback = verify_callback();

    _context = std::move(ssl_context(static_cast<ssl_context::method>(_method)));
}

bool encryption::is_enabled() const
{
    return _enabled;
}

void encryption::set_enabled(bool enabled)
{
    _enabled = enabled;
}

std::string encryption::ca_filename() const
{
    return _ca_filename;
}

void encryption::set_ca_filename(const std::string& filename)
{
    _last_error.clear();

    if (!filename.empty())
    {
        auto file = helper::read_binary_file(filename);

        if (file.second > 0)
        {
            _context.add_certificate_authority(const_buffer(file.first.get(), file.second));
        }
        else
        {
            _last_error = "Can't read CA file";
        }
    }
}

std::string encryption::cert_filename() const
{
    return _cert_filename;
}

void encryption::set_cert_filename(const std::string& filename)
{
    _cert_filename = filename;

    _context.use_certificate_file(filename, ssl_context::pem);
}

std::string encryption::private_key_filename() const
{
    return _private_key_filename;
}

void encryption::set_private_key_filename(const std::string& filename)
{
    _private_key_filename = filename;

    _context.use_private_key_file(filename, ssl_context::pem);
}

int encryption::verify_mode() const
{
    return _verify_mode;
}

void encryption::set_verify_mode(int mode)
{
    _verify_mode = mode;
}

void encryption::set_verify_callback(verify_callback callback)
{
    _verify_callback = callback;
}

bool encryption::verify_certificate(bool preverified, verify_context& context)
{
    if (_verify_callback)
    {
        return _verify_callback(preverified, context.native_handle());
    }

    return preverified;
}

std::string encryption::last_error() const
{
    return _last_error;
}

encryption::ssl_context& encryption::context()
{
    return _context;
}
}
