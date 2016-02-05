#include "client.h"

namespace lnetlib
{
client::client()
    : _running(false), _encryption(new encryption())
{
}

client::~client()
{
}

std::shared_ptr<encryption> client::encryption_info() const
{
    return _encryption;
}

void client::connect(const std::string& hostname, int port)
{
    if (_running)
    {
        return;
    }

    if (!_encryption->last_error().empty())
    {
        error(-1, _encryption->last_error());

        return;
    }

    _running = true;

    tcp::resolver resolver(_service);
    tcp::resolver::query resolver_query(hostname, std::to_string(port));

    _endpoint = resolver.resolve(resolver_query);

    try_connect();
}

void client::disconnect()
{
    if (!_running)
    {
        return;
    }

    _service.stop();

    _running = false;
}

std::shared_ptr<connection> client::conn() const
{
    return _connection;
}

void client::try_connect()
{
    create_session();

    auto thread_func = [this]()
    {
        _service.run();
    };

    std::thread thread(thread_func);

    thread.detach();
}

void client::create_session()
{
    client_session *session = nullptr;

    if (!_encryption->is_enabled())
    {
        session = new client_session_non_ssl(_service, _endpoint);
    }
    else
    {
        session = new client_session_ssl(_service, _endpoint, _encryption);
    }

    auto handler = std::bind(&client::connected_handler, this, session, std::placeholders::_1, std::placeholders::_2);

    session->connected.connect(handler);

    session->start();
}

void client::connected_handler(client_session *session, std::shared_ptr<connection> conn, const error_code& err)
{
    if (err)
    {
        switch (err.value())
        {
            case boost::asio::error::connection_refused:
                refused();
                break;

            default:
                error(err.value(), err.message());
                break;

        }

        return;
    }

    _connection = conn;

    _session = std::shared_ptr<client_session>(session);

    auto closed_slot = [this](std::shared_ptr<connection> conn)
    {
        (void)conn;

        disconnected();
    };

    auto error_slot = [this](std::shared_ptr<connection> conn, int code, const std::string& message)
    {
        (void)conn;

        error(code, message);
    };

    auto received_slot = [this](std::shared_ptr<connection> conn, std::unique_ptr<istream> stream)
    {
        (void)conn;

        received(std::move(stream));
    };

    _connection->closed.connect(closed_slot);
    _connection->error.connect(error_slot);
    _connection->received.connect(received_slot);

    connected();
}

void client::error_handler(std::shared_ptr<connection> conn, int code, const std::string& message)
{
    (void)conn;

    error(code, message);
}

void client::received_handler(std::shared_ptr<connection> conn, std::unique_ptr<istream> stream)
{
    (void)conn;

    received(std::move(stream));
}
}
