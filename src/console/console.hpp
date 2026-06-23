#pragma once
#include "task/manager.hpp"
#include "file_manager/fileManager.hpp"
#include "settings.hpp"
#include "utils/utils.hpp"

#include <ctime>
#include <iostream>
#include <string>
#include <map>
#include <functional>

class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
};

class Console {
    private:
        std::shared_ptr<TaskManager> taskManager_;
        std::shared_ptr<FileManager> fileManager_;

        // Command handlers, merge these with the cli_handler later
        void handleAddTask();
        void handleListTasks();
        void handleCompleteTask();
        void handleFileOperations();
        void handleExit();
        void handleAddTag();
        void handleRemoveTag();

        // Command dispatcher map
        std::map<std::string, std::function<void()>> commandMap_;

        void initializeCommands();
        
    public:
        Console(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm);
        
        void displayMenu();
        
        void handleInput(std::string input);
        
        // Getters for private members
        std::shared_ptr<TaskManager> getTaskManager();
        std::shared_ptr<FileManager> getFileManager();
};