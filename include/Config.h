#pragma once

#include "Types.h"

#include <string>
#include <vector>

namespace lb {

struct RuntimeConfig {
    StrategyType strategy = StrategyType::ROUND_ROBIN;
    std::size_t clientCount = 1;
    std::size_t requestsPerClient = 10;
    std::size_t requestRate = 100;
    std::size_t simulationDurationMs = 1000;
    std::size_t processingMinMs = 10;
    std::size_t processingMaxMs = 50;
    double failureProbability = 0.05;
    double serverFailureProbability = 0.02;
    std::string host = "127.0.0.1";
    int port = 8080;
};

RuntimeConfig parseConfig(const std::vector<std::string>& args);

} // namespace lb
