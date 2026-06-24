#include "logger.hpp"
#include "ui.hpp"
#include <iostream>
#include <ctime>

void Logger::log(LogLevel level, const std::string& message) {
    switch(level){
        case LogLevel::ERROR:
            UI::instance().println("[ERROR] " + message, Color::Red);
            break;
        case LogLevel::WARNING:
            UI::instance().println("[WARNING] " + message, Color::Yellow);
            break;
        case LogLevel::INFO:
            UI::instance().println("[INFO] " + message, Color::Blue);
            break;
        case LogLevel::FATAL:
            UI::instance().println("[FATAL] " + message, Color::Red);
            break;
    }
}