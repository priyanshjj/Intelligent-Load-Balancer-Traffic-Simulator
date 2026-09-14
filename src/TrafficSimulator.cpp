#include "TrafficSimulator.h"

#include <chrono>
#include <random>
#include <thread>

namespace lb {

TrafficSimulator::TrafficSimulator(LoadBalancer& balancer) : balancer_(balancer) {}

TrafficSimulator::~TrafficSimulator() {
    stop();
}

void TrafficSimulator::configure(const SimulationConfig& config) {
    config_ = config;
}

void TrafficSimulator::start() {
    running_ = true;
    runner_ = std::thread([this]() { run(); });
}

void TrafficSimulator::stop() {
    running_ = false;
    cv_.notify_all();
    if (runner_.joinable()) {
        runner_.join();
    }
}

void TrafficSimulator::clientWorker(std::size_t clientId) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> payloadDist(128, 4096);
    std::uniform_int_distribution<std::size_t> processDist(config_.processingMinMs, config_.processingMaxMs);

    for (std::size_t i = 0; i < config_.requestsPerClient && running_; ++i) {
        auto request = balancer_.routeRequest(clientId, payloadDist(gen), processDist(gen));
        if (request.status == RequestStatus::SUCCESS || request.status == RequestStatus::FAILED || request.status == RequestStatus::REJECTED) {
            totalRequests_.fetch_add(1, std::memory_order_relaxed);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void TrafficSimulator::run() {
    std::vector<std::thread> clients;
    clients.reserve(config_.clientCount);
    for (std::size_t i = 0; i < config_.clientCount; ++i) {
        clients.emplace_back(&TrafficSimulator::clientWorker, this, i + 1);
    }
    for (auto& t : clients) {
        if (t.joinable()) {
            t.join();
        }
    }
}

std::size_t TrafficSimulator::totalRequests() const {
    return totalRequests_.load(std::memory_order_relaxed);
}

} // namespace lb
