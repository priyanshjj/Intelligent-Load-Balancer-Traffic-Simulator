#include "Config.h"

#include <cstddef>
#include <stdexcept>

namespace lb {

RuntimeConfig parseConfig(const std::vector<std::string>& args) {
    RuntimeConfig config;
    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--strategy") {
            if (i + 1 < args.size()) {
                std::string strategy = args[++i];
                if (strategy == "round_robin" || strategy == "rr") {
                    config.strategy = StrategyType::ROUND_ROBIN;
                } else if (strategy == "least_connections" || strategy == "lc") {
                    config.strategy = StrategyType::LEAST_CONNECTIONS;
                }
            }
        } else if (arg == "--clients") {
            if (i + 1 < args.size()) {
                config.clientCount = static_cast<std::size_t>(std::stoul(args[++i]));
            }
        } else if (arg == "--rate") {
            if (i + 1 < args.size()) {
                config.requestRate = static_cast<std::size_t>(std::stoul(args[++i]));
            }
        } else if (arg == "--requests") {
            if (i + 1 < args.size()) {
                config.requestsPerClient = static_cast<std::size_t>(std::stoul(args[++i]));
            }
        } else if (arg == "--port") {
            if (i + 1 < args.size()) {
                config.port = std::stoi(args[++i]);
            }
        }
    }
    return config;
}

} // namespace lb
