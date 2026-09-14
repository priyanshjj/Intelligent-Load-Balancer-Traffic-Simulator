#pragma once

#include "LoadBalancer.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace lb {

struct SimulationConfig {
    std::size_t clientCount = 1;
    std::size_t requestsPerClient = 10;
    std::size_t requestRate = 100;
    std::size_t simulationDurationMs = 1000;
    std::size_t processingMinMs = 10;
    std::size_t processingMaxMs = 50;
    double failureProbability = 0.05;
    double serverFailureProbability = 0.02;
};

class TrafficSimulator {
public:
    explicit TrafficSimulator(LoadBalancer& balancer);
    ~TrafficSimulator();

    void configure(const SimulationConfig& config);
    void start();
    void stop();
    void run();
    std::size_t totalRequests() const;

private:
    void clientWorker(std::size_t clientId);

    LoadBalancer& balancer_;
    SimulationConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<std::size_t> totalRequests_{0};
    std::thread runner_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

} // namespace lb
