#include "LoadBalancingStrategy.h"

#include <algorithm>
#include <limits>
#include <mutex>

namespace lb {

BackendServer* RoundRobinStrategy::selectServer(const std::vector<BackendServer*>& servers) {
    if (servers.empty()) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::size_t start = index_ % servers.size();
    for (std::size_t offset = 0; offset < servers.size(); ++offset) {
        auto* server = servers[(start + offset) % servers.size()];
        if (server && server->isHealthy()) {
            index_ = (start + offset + 1) % servers.size();
            return server;
        }
    }
    return nullptr;
}

BackendServer* LeastConnectionsStrategy::selectServer(const std::vector<BackendServer*>& servers) {
    BackendServer* best = nullptr;
    std::size_t bestConnections = std::numeric_limits<std::size_t>::max();

    for (auto* server : servers) {
        if (!server || !server->isHealthy()) {
            continue;
        }

        const auto active = server->getActiveConnections();
        if (best == nullptr || active < bestConnections) {
            best = server;
            bestConnections = active;
        }
    }

    return best;
}

} // namespace lb
