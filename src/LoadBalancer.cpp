#include "LoadBalancer.h"

#include <algorithm>

namespace lb {

LoadBalancer::LoadBalancer() : strategy_(std::make_unique<RoundRobinStrategy>()) {}

LoadBalancer::~LoadBalancer() = default;

void LoadBalancer::addServer(BackendServer* server) {
    if (!server) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    servers_.push_back(server);
}

void LoadBalancer::removeServer(std::uint64_t serverId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(servers_.begin(), servers_.end(),
        [serverId](BackendServer* server) { return server && server->getId() == serverId; });
    servers_.erase(it, servers_.end());
}

void LoadBalancer::setStrategy(std::unique_ptr<LoadBalancingStrategy> strategy) {
    if (!strategy) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    strategy_ = std::move(strategy);
}

Request LoadBalancer::routeRequest(std::uint64_t clientId, std::size_t payloadSize, std::size_t processingTimeMs) {
    Request request{};
    request.id = nextRequestId_++;
    request.clientId = clientId;
    request.payloadSize = payloadSize;
    request.processingTimeMs = processingTimeMs;
    request.status = RequestStatus::QUEUED;

    std::lock_guard<std::mutex> lock(mutex_);
    BackendServer* server = nullptr;
    if (strategy_) {
        server = strategy_->selectServer(servers_);
    }

    if (!server || !server->isHealthy()) {
        request.status = RequestStatus::REJECTED;
        return request;
    }

    request.selectedBackend = server->getAddress();
    request.status = RequestStatus::PROCESSING;
    std::size_t responseTimeMs = 0;
    bool accepted = server->processRequest(payloadSize, processingTimeMs, responseTimeMs);
    request.responseTimeMs = responseTimeMs;
    request.status = accepted ? RequestStatus::SUCCESS : RequestStatus::FAILED;
    return request;
}

std::vector<BackendServer*> LoadBalancer::getHealthyServers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BackendServer*> healthy;
    for (auto* server : servers_) {
        if (server && server->isHealthy()) {
            healthy.push_back(server);
        }
    }
    return healthy;
}

std::size_t LoadBalancer::serverCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return servers_.size();
}

} // namespace lb
