#pragma once

#include "Types.h"

#include <cstddef>
#include <mutex>

namespace lb {

struct MetricsSnapshot {
    std::size_t totalRequests = 0;
    std::size_t successfulRequests = 0;
    std::size_t failedRequests = 0;
    std::size_t rejectedRequests = 0;
    std::size_t totalLatencyMs = 0;
    double averageLatencyMs = 0.0;
};

class MetricsCollector {
public:
    void recordRequest(const Request& request);
    MetricsSnapshot snapshot() const;

private:
    mutable std::mutex mutex_;
    std::size_t totalRequests_ = 0;
    std::size_t successfulRequests_ = 0;
    std::size_t failedRequests_ = 0;
    std::size_t rejectedRequests_ = 0;
    std::size_t totalLatencyMs_ = 0;
};

} // namespace lb
