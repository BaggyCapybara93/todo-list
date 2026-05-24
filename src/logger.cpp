#include "logger.hpp"
#include <iostream>
#include <ctime>

void Logger::log(LogLevel level, const std::string& message) {
    switch(level){
        case LogLevel::ERROR:
            std::cerr << "[ERROR] " << message << std::endl;
            break;
        case LogLevel::WARNING:
            std::cerr << "[WARNING] " << message << std::endl;
            break;
        case LogLevel::INFO:
            std::cout << "[INFO] " << message << std::endl;
            break;
        case LogLevel::FATAL:
            std::cerr << "[FATAL] " << message << std::endl;
            break;
    }
}