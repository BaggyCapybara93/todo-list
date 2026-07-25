#pragma once
#include <string>

class Settings{
    private:
        int maxTasksPerFile = 1000;
        bool enableAutosave = true;
        bool verbose = false;
        std::string todoFilePath = ".todo_list.txt";
        std::string settingsFilePath = ".settings.txt";

    public:
        Settings(){};
        ~Settings() = default;

        //Getters
        const int& getMaxTasksPerFile() const{return maxTasksPerFile;}
        const bool& getEnableAutosave() const{return enableAutosave;}
        const bool& getVerbose() const{return verbose;}
        const std::string& getTodoFilePath() const{return todoFilePath;}
        const std::string& getSettingsFilePath() const{return settingsFilePath;}

        //Setters   
        void setMaxTasksPerFile(const int& value){maxTasksPerFile = value;}
        void setEnableAutosave(const bool& value){enableAutosave = value;}
        void setVerbose(const bool& value){verbose = value;}
        void setTodoFilePath(const std::string& value){todoFilePath = value;}
        void setSettingsFilePath(const std::string& value){settingsFilePath = value;}

        void loadSettings(const std::string& settingsFilePath = "settings.txt");

        void saveSettings(const std::string& settingsFilePath = "settings.txt");
};