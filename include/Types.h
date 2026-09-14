#pragma once

#include <cstdint>
#include <string>

namespace lb {

enum class ServerStatus {
    HEALTHY,
    UNHEALTHY,
    RECOVERING,
    STOPPED
};

enum class RequestStatus {
    QUEUED,
    PROCESSING,
    SUCCESS,
    FAILED,
    TIMEOUT,
    REJECTED
};

enum class StrategyType {
    ROUND_ROBIN,
    LEAST_CONNECTIONS
};

struct ServerAddress {
    std::string host;
    int port = 8080;
};

struct Request {
    std::uint64_t id = 0;
    std::uint64_t clientId = 0;
    std::uint64_t timestamp = 0;
    std::size_t payloadSize = 0;
    std::size_t processingTimeMs = 0;
    std::string selectedBackend;
    RequestStatus status = RequestStatus::QUEUED;
    std::size_t responseTimeMs = 0;
};
} // namespace lb
