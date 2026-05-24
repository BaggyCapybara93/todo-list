#pragma once
#include "taskManager.hpp"
#include "fileManager.hpp"
#include <map>
#include <functional>

class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
};

class ConsoleUI {
private:
    TaskManager& taskManager_;
    FileManager& fileManager_;

    // Command handlers
    void handleAddTask();
    void handleListTasks();
    void handleCompleteTask();
    void handleFileOperations();
    void handleExit();

    // Command dispatcher map
    std::map<std::string, std::function<void()>> commandMap_;

    void initializeCommands();
    
public:
    ConsoleUI(TaskManager& tm, FileManager& fm);
    
    void displayMenu();
    
    void handleInput(std::string input);
    
    // Getters for private members
    TaskManager& getTaskManager();
    FileManager& getFileManager();
};