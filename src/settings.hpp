#pragma once
#include <string>

class Settings{
    private:
        int maxTasksPerFile = 1000;
        bool enableAutosave = true;

        bool verbose = false;

    public:
        Settings(){};
        ~Settings() = default;

        //Getters
        const int& getMaxTasksPerFile() const{return maxTasksPerFile;}
        const bool& getEnableAutosave() const{return enableAutosave;}
        const bool& getVerbose() const{return verbose;}

        //Setters   
        void setMaxTasksPerFile(const int& value){maxTasksPerFile = value;}
        void setEnableAutosave(const bool& value){enableAutosave = value;}
        void setVerbose(const bool& value){verbose = value;}

        void loadSettings(const std::string& settingsFilePath = "settings.txt");

        void saveSettings(const std::string& settingsFilePath = "settings.txt");
};