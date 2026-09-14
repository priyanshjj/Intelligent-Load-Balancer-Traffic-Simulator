#pragma once

#include "BackendServer.h"
#include "LoadBalancingStrategy.h"
#include "Types.h"

#include <memory>
#include <mutex>
#include <vector>

namespace lb {

class LoadBalancer {
public:
    LoadBalancer();
    ~LoadBalancer();

    void addServer(BackendServer* server);
    void removeServer(std::uint64_t serverId);
    void setStrategy(std::unique_ptr<LoadBalancingStrategy> strategy);
    Request routeRequest(std::uint64_t clientId, std::size_t payloadSize, std::size_t processingTimeMs);
    std::vector<BackendServer*> getHealthyServers() const;
    std::size_t serverCount() const;

private:
    std::vector<BackendServer*> servers_;
    std::unique_ptr<LoadBalancingStrategy> strategy_;
    mutable std::mutex mutex_;
    std::uint64_t nextRequestId_ = 1;
};

} // namespace lb
