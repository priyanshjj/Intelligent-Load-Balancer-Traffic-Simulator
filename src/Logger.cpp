#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lb {

namespace {
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        default: return "INFO";
    }
}
} // namespace

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setFileOutput(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(path, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream stream;
    stream << "[" << std::put_time(&tm, "%H:%M:%S") << "] [" << levelToString(level) << "] " << message;

    std::cout << stream.str() << std::endl;
    if (logFile_.is_open()) {
        logFile_ << stream.str() << std::endl;
    }
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

} // namespace lb
