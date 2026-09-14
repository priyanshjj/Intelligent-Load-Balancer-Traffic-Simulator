#pragma once

#include <fstream>
#include <mutex>
#include <string>

namespace lb {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& instance();

    void setFileOutput(const std::string& path);
    void log(LogLevel level, const std::string& message);

private:
    Logger() = default;
    ~Logger();

    std::mutex mutex_;
    std::ofstream logFile_;
};

} // namespace lb
