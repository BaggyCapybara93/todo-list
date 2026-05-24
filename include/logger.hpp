#pragma once
#include <string>

class Logger {
    private:

    public:
        enum class LogLevel { ERROR, WARNING, INFO, FATAL};

        static void log(LogLevel level, const std::string& message);
        
};