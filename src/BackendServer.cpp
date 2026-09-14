#include "BackendServer.h"

#include <chrono>
#include <thread>

namespace lb {

std::uint64_t BackendServer::nextId() {
    static std::uint64_t counter = 1;
    return counter++;
}

BackendServer::BackendServer(std::string host, int port, std::size_t maxConnections)
    : id_(nextId()), host_(std::move(host)), port_(port), maxConnections_(maxConnections),
      activeConnections_(0), totalRequests_(0), successfulRequests_(0), failedRequests_(0),
      processingTimeMs_(0), latencyMs_(0), status_(ServerStatus::HEALTHY), utilization_(0.0),
      healthCheckTimestampMs_(0) {}

std::uint64_t BackendServer::getId() const { return id_; }
std::string BackendServer::getAddress() const { return host_ + ":" + std::to_string(port_); }
int BackendServer::getPort() const { return port_; }

void BackendServer::setStatus(ServerStatus status) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = status;
}

ServerStatus BackendServer::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

bool BackendServer::isHealthy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_ == ServerStatus::HEALTHY || status_ == ServerStatus::RECOVERING;
}

void BackendServer::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = ServerStatus::HEALTHY;
}

void BackendServer::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = ServerStatus::STOPPED;
    activeConnections_ = 0;
}

void BackendServer::simulateFailure() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = ServerStatus::UNHEALTHY;
}

void BackendServer::recover() {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = ServerStatus::RECOVERING;
    if (activeConnections_ < maxConnections_) {
        status_ = ServerStatus::HEALTHY;
    }
}

void BackendServer::incrementConnections() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeConnections_ < maxConnections_) {
        ++activeConnections_;
    }
}

void BackendServer::decrementConnections() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activeConnections_ > 0) {
        --activeConnections_;
    }
}

std::size_t BackendServer::getActiveConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeConnections_;
}

bool BackendServer::processRequest(std::size_t payloadSize, std::size_t processingTimeMs, std::size_t& responseTimeMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != ServerStatus::HEALTHY && status_ != ServerStatus::RECOVERING) {
        return false;
    }

    if (activeConnections_ >= maxConnections_) {
        return false;
    }

    ++activeConnections_;
    ++totalRequests_;
    processingTimeMs_ = processingTimeMs;
    latencyMs_ = 2 + payloadSize / 64;
    responseTimeMs = processingTimeMs + latencyMs_;
    successfulRequests_ += 1;
    utilization_ = static_cast<double>(processingTimeMs) / std::max<std::size_t>(1, processingTimeMs + 10);
    --activeConnections_;
    return true;
}

ServerStatistics BackendServer::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    ServerStatistics stats;
    stats.id = id_;
    stats.host = host_;
    stats.port = port_;
    stats.maxConnections = maxConnections_;
    stats.activeConnections = activeConnections_;
    stats.totalRequests = totalRequests_;
    stats.successfulRequests = successfulRequests_;
    stats.failedRequests = failedRequests_;
    stats.processingTimeMs = processingTimeMs_;
    stats.latencyMs = latencyMs_;
    stats.status = status_;
    stats.utilization = utilization_;
    stats.healthCheckTimestampMs = healthCheckTimestampMs_;
    return stats;
}

void BackendServer::setHealthCheckTimestamp(std::size_t timestampMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    healthCheckTimestampMs_ = timestampMs;
}

double BackendServer::getUtilization() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return utilization_;
}

} // namespace lb
