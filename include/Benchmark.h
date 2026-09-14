#pragma once

#include "BackendServer.h"

#include <cstddef>
#include <string>
#include <vector>

namespace lb {

struct BenchmarkResult {
    std::string strategyName;
    std::size_t totalRequests = 0;
    std::size_t successfulRequests = 0;
    std::size_t failedRequests = 0;
    double averageLatencyMs = 0.0;
};

class BenchmarkRunner {
public:
    static std::vector<BenchmarkResult> compareStrategies(const std::vector<BackendServer*>& servers,
                                                        std::size_t iterations,
                                                        std::size_t payloadSize);
};

} // namespace lb
