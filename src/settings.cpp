#include "settings.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Default settings
Settings setting_;

// Load settings from file
bool loadSettings(const std::string& settingsFilePath) {
    std::ifstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cout << "No existing settings file found. Using defaults." << std::endl;
        return false;
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
                    setting_.maxTasksPerFile = std::stoi(value);
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to parse line " << lineNum << ": invalid value '" << value << "' for max_tasks_per_file" << std::endl;
                }
            } else if (key == "enable_autosave") {
                try {
                    setting_.enableAutosave = (value == "true" || value == "1");
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to parse line " << lineNum << ": invalid value '" << value << "' for enable_autosave" << std::endl;
                }
            } else {
                // Unknown key - skip it with a warning
                std::cerr << "Warning: Unknown setting key '" << key << "' on line " << lineNum << ", skipping." << std::endl;
            }
        }
    }

    return true;
}

// Save settings to file
bool saveSettings(const std::string& settingsFilePath) {
    std::ofstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open settings file for writing." << std::endl;
        return false;
    }

    file << "max_tasks_per_file=" << setting_.maxTasksPerFile << std::endl;
    file << "enable_autosave=" << (setting_.enableAutosave ? "true" : "false") << std::endl;

    return true;
}

// Get default settings path
std::string getDefaultSettingsPath() {
    return "settings.txt";
}
