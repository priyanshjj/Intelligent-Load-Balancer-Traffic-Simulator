#pragma once

#include "Types.h"

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

namespace lb {

struct ServerStatistics {
    std::uint64_t id = 0;
    std::string host;
    int port = 0;
    std::size_t maxConnections = 0;
    std::size_t activeConnections = 0;
    std::size_t totalRequests = 0;
    std::size_t successfulRequests = 0;
    std::size_t failedRequests = 0;
    std::size_t processingTimeMs = 0;
    std::size_t latencyMs = 0;
    ServerStatus status = ServerStatus::HEALTHY;
    double utilization = 0.0;
    std::size_t healthCheckTimestampMs = 0;
};

class BackendServer {
public:
    BackendServer(std::string host, int port, std::size_t maxConnections = 10);

    std::uint64_t getId() const;
    std::string getAddress() const;
    int getPort() const;
    void setStatus(ServerStatus status);
    ServerStatus getStatus() const;
    bool isHealthy() const;
    void start();
    void stop();
    void simulateFailure();
    void recover();
    void incrementConnections();
    void decrementConnections();
    std::size_t getActiveConnections() const;
    bool processRequest(std::size_t payloadSize, std::size_t processingTimeMs, std::size_t& responseTimeMs);
    ServerStatistics getStatistics() const;
    void setHealthCheckTimestamp(std::size_t timestampMs);
    double getUtilization() const;

private:
    static std::uint64_t nextId();

    std::uint64_t id_;
    std::string host_;
    int port_;
    std::size_t maxConnections_;
    std::size_t activeConnections_;
    std::size_t totalRequests_;
    std::size_t successfulRequests_;
    std::size_t failedRequests_;
    std::size_t processingTimeMs_;
    std::size_t latencyMs_;
    ServerStatus status_;
    double utilization_;
    std::size_t healthCheckTimestampMs_;
    mutable std::mutex mutex_;
};

} // namespace lb
