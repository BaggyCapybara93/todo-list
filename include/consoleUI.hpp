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
    std::shared_ptr<TaskManager> taskManager_;
    std::shared_ptr<FileManager> fileManager_;

    // Command handlers
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
    ConsoleUI(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm);
    
    void displayMenu();
    
    void handleInput(std::string input);
    
    // Getters for private members
    std::shared_ptr<TaskManager> getTaskManager();
    std::shared_ptr<FileManager> getFileManager();
};