#pragma once
#include <string>

struct Settings{
    int maxTasksPerFile = 1000;
    bool enableAutosave = true;
};

extern Settings setting_;

bool loadSettings(const std::string& settingsFilePath = "settings.txt");

bool saveSettings(const std::string& settingsFilePath = "settings.txt");

std::string getDefaultSettingsPath(); 