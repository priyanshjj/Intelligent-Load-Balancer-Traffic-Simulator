#include "Logger.h"

#include <iostream>

int main() {
    lb::Logger::instance().log(lb::LogLevel::INFO, "Load balancer project initialized.");
    std::cout << "Intelligent Load Balancer & Traffic Simulator" << std::endl;
    return 0;
}
