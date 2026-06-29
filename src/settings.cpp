#include "settings.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "logger.hpp"

// Load settings from file
void Settings::loadSettings(const std::string& settingsFilePath) {
    std::ifstream file(settingsFilePath);
    if (!file.is_open()) {
        if(verbose) Logger::log(Logger::LogLevel::INFO, "No existing settings file found. Using defaults.");
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == "max_tasks_per_file") {
                try {
                    maxTasksPerFile = std::stoi(value);
                } catch (const std::exception& e) {
                    Logger::log(Logger::LogLevel::WARNING, "Failed to parse line " + std::to_string(lineNum) + ": invalid value '" + value + "' for max_tasks_per_file");
                }
            } else if (key == "enable_autosave") {
                try {
                    enableAutosave = (value == "true" || value == "1");
                } catch (const std::exception& e) {
                    Logger::log(Logger::LogLevel::WARNING, "Failed to parse line " + std::to_string(lineNum) + ": invalid value '" + value + "' for enable_autosave");
                }
            } else if (key == "verbose") {
                try {
                    verbose = (value == "true" || value == "1");
                } catch (const std::exception& e) {
                    Logger::log(Logger::LogLevel::WARNING, "Failed to parse line " + std::to_string(lineNum) + ": invalid value '" + value + "' for verbose");
                }
            }else {
                // Unknown key - skip it with a warning
                Logger::log(Logger::LogLevel::WARNING, "Unknown setting key '" + key + "' on line " + std::to_string(lineNum) + ", skipping.");
            }
        }
    }
}

// Save settings to file
void Settings::saveSettings(const std::string& settingsFilePath) {
    std::ofstream file(settingsFilePath);
    if (!file.is_open()) {
        Logger::log(Logger::LogLevel::ERROR, "Failed to open settings file for writing.");
    }

    file << "max_tasks_per_file=" << maxTasksPerFile << std::endl;
    file << "enable_autosave=" << (enableAutosave ? "true" : "false") << std::endl;
    file << "verbose=" << (verbose ? "true" : "false") << std::endl;
}

