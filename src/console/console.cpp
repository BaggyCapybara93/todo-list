#include "console.hpp"

Console::Console(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm) : taskManager_(std::move(tm)), fileManager_(std::move(fm)) {
    initializeCommands();
}

// Initialize the command map
void Console::initializeCommands() {
    commandMap_["1"] = [this]() { this->handleAddTask(); };
    commandMap_["2"] = [this]() { this->handleListTasks(); };
    commandMap_["3"] = [this]() { this->handleCompleteTask(); };
    commandMap_["4"] = [this]() { this->handleAddTag(); };
    commandMap_["5"] = [this]() { this->handleRemoveTag(); };
    commandMap_["6"] = [this]() { this->handleSetRepeatTask(); };
    commandMap_["8"] = [this]() { this->handleFileOperations(); };
    commandMap_["9"] = [this]() { this->handleExit(); };
}

std::shared_ptr<TaskManager> Console::getTaskManager() {
    return taskManager_;
}

std::shared_ptr<FileManager> Console::getFileManager() {
    return fileManager_;
}