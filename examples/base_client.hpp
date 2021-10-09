#include "brynet/base/AppStatus.hpp"
#include <brynet/net/wrapper/ConnectionBuilder.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/TcpService.hpp>
#include <optional>

using namespace std::literals::string_view_literals;
using namespace brynet::net;


struct BrynetLdapReader {
    brynet::base::BasePacketReader& reader;

    size_t size() {
        return reader.getLeft();
    }

    bool empty() {
        return this->size() == 0;
    }

    std::optional<uint8_t> read() {
        if (empty())
            return std::nullopt;
        return reader.readUINT8();
    }

    std::optional<std::string_view> read(size_t length) {
        if(length > this->size())
            return std::nullopt;
        auto sv = std::string_view(reader.currentBuffer(), length);
        reader.addPos(length);
        return sv;
    }
};

class LDAPClient {
    private:
    std::shared_ptr<TcpService> service;
    std::shared_ptr<AsyncConnector> connector;

    public:
    std::shared_ptr<TcpConnection> connection;

    LDAPClient(std::string ip, uint16_t port) {
        this->service = TcpService::Create();
        service->startWorkerThread(2);
        this->connector = AsyncConnector::Create();
        connector->startWorkerThread();

        wrapper::ConnectionBuilder connectionBuilder;
        connectionBuilder
                .WithService(service)
                .WithConnector(connector)
                .WithMaxRecvBufferSize(1024 * 1024);

        this->connection = connectionBuilder
                .WithAddr(ip, port)
                .WithTimeout(std::chrono::seconds(10))
                .syncConnect();
    }

    ~LDAPClient() {
        this->connection->postShutdown();
        this->connector->stopWorkerThread();
        this->service->stopWorkerThread();
    }
};