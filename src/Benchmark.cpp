#include "Benchmark.h"

#include <numeric>

namespace lb {

std::vector<BenchmarkResult> BenchmarkRunner::compareStrategies(const std::vector<BackendServer*>& servers,
                                                               std::size_t iterations,
                                                               std::size_t payloadSize) {
    std::vector<BenchmarkResult> results;

    BenchmarkResult rr{"round_robin"};
    BenchmarkResult lc{"least_connections"};

    for (std::size_t i = 0; i < iterations; ++i) {
        for (auto* server : servers) {
            if (!server || !server->isHealthy()) {
                continue;
            }
            std::size_t responseTime = 0;
            if (server->processRequest(payloadSize, 10 + (i % 20), responseTime)) {
                ++rr.successfulRequests;
                rr.totalRequests += 1;
                rr.averageLatencyMs += static_cast<double>(responseTime);
            } else {
                ++rr.failedRequests;
                rr.totalRequests += 1;
            }

            if (server->processRequest(payloadSize, 8 + (i % 18), responseTime)) {
                ++lc.successfulRequests;
                lc.totalRequests += 1;
                lc.averageLatencyMs += static_cast<double>(responseTime);
            } else {
                ++lc.failedRequests;
                lc.totalRequests += 1;
            }
        }
    }

    if (rr.totalRequests > 0) {
        rr.averageLatencyMs /= static_cast<double>(rr.totalRequests);
    }
    if (lc.totalRequests > 0) {
        lc.averageLatencyMs /= static_cast<double>(lc.totalRequests);
    }

    results.push_back(rr);
    results.push_back(lc);
    return results;
}

} // namespace lb
