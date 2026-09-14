#include "MetricsCollector.h"

#include <algorithm>

namespace lb {

void MetricsCollector::recordRequest(const Request& request) {
    std::lock_guard<std::mutex> lock(mutex_);
    ++totalRequests_;
    if (request.status == RequestStatus::SUCCESS) {
        ++successfulRequests_;
    } else if (request.status == RequestStatus::FAILED) {
        ++failedRequests_;
    } else if (request.status == RequestStatus::REJECTED) {
        ++rejectedRequests_;
    }
    totalLatencyMs_ += request.responseTimeMs;
}

MetricsSnapshot MetricsCollector::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    MetricsSnapshot snapshot;
    snapshot.totalRequests = totalRequests_;
    snapshot.successfulRequests = successfulRequests_;
    snapshot.failedRequests = failedRequests_;
    snapshot.rejectedRequests = rejectedRequests_;
    snapshot.totalLatencyMs = totalLatencyMs_;
    if (totalRequests_ > 0) {
        snapshot.averageLatencyMs = static_cast<double>(totalLatencyMs_) / static_cast<double>(totalRequests_);
    }
    return snapshot;
}

} // namespace lb
