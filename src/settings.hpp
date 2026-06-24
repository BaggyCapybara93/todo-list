#pragma once
#include <string>

struct Settings{
    int maxTasksPerFile;
    bool enableAutosave ;

    bool verbose;

    Settings() : maxTasksPerFile(1000), enableAutosave(true), verbose(false) {}
};

extern Settings setting_;

Settings loadSettings(const std::string& settingsFilePath = "settings.txt");

Settings saveSettings(const std::string& settingsFilePath = "settings.txt");

std::string getDefaultSettingsPath(); 