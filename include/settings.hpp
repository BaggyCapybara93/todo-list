#pragma once
#include <string>

struct Settings{
    int maxTasksPerFile = 1000;
    bool enableAutosave = true;

    Settings() : maxTasksPerFile(1000), enableAutosave(true) {}
};

extern Settings setting_;

Settings loadSettings(const std::string& settingsFilePath = "settings.txt");

Settings saveSettings(const std::string& settingsFilePath = "settings.txt");

std::string getDefaultSettingsPath(); 