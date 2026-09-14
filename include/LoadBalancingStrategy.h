#pragma once

#include "BackendServer.h"

#include <mutex>
#include <vector>

namespace lb {

class LoadBalancingStrategy {
public:
    virtual ~LoadBalancingStrategy() = default;
    virtual BackendServer* selectServer(const std::vector<BackendServer*>& servers) = 0;
};

class RoundRobinStrategy : public LoadBalancingStrategy {
public:
    BackendServer* selectServer(const std::vector<BackendServer*>& servers) override;

private:
    std::size_t index_ = 0;
    std::mutex mutex_;
};

class LeastConnectionsStrategy : public LoadBalancingStrategy {
public:
    BackendServer* selectServer(const std::vector<BackendServer*>& servers) override;
};

} // namespace lb
