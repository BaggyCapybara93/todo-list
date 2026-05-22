#include "taskManager.hpp"
#include <iostream>
#include "settings.hpp"

// Constructor implementation
TaskManager::TaskManager(FileManager& fm) : nextId_(1), fileManager_(fm) {
    loadTasks();
}

// Destructor to save tasks if autosave is enabled
TaskManager::~TaskManager() {
    try {
        if (setting_.enableAutosave) {
            saveTasks();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in destructor: " << e.what() << std::endl;
    }
}

void TaskManager::loadTasks() {
    try{
        auto tasksOpt = fileManager_.loadTodoList();
        if (tasksOpt) {
            tasks_.clear();
            for (const auto& task : *tasksOpt) {
                if (setting_.maxTasksPerFile > 0 && tasks_.size() >= static_cast<size_t>(setting_.maxTasksPerFile)) {
                    std::cerr << "Warning: Maximum tasks per file limit reached. Some tasks may not be loaded.\n";
                    break;
                }

                if (getTask(task->getId()).has_value()){
                    std::cerr << "Warning: Duplicate task ID " << task->getId() << " found in file. Skipping.\n";
                    continue;
                }

                tasks_[task->getId()] = task;
            }
            std::cout << "Tasks loaded successfully." << std::endl;
        } else {
            std::cerr << "Could not load tasks. Starting with an empty list." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading tasks: " << e.what() << std::endl;
    }
}

void TaskManager::saveTasks() {
    try{
        std::vector<std::shared_ptr<Task>> tasksToSave;
        for (const auto& pair : tasks_) {
            tasksToSave.push_back(pair.second);
        }
        if (fileManager_.saveTodoList(tasksToSave)) {
            std::cout << "Tasks saved successfully." << std::endl;
        } else {
            std::cerr << "Failed to save tasks." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving tasks: " << e.what() << std::endl;
    }
}

void TaskManager::addTask(const std::string& name, const std::string& description, int priority, const std::chrono::system_clock::time_point& dueDate) {
    try{
        int taskId = nextId_++;

        if(priority < 0 || priority > 10) {
            std::cerr << "Error: Priority must be between 0 and 10." << std::endl;
            return;
        }

        auto newTask = std::make_shared<Task>(taskId, name, description, false, priority, dueDate);
        tasks_[taskId] = newTask;
        saveTasks();
    } catch (const std::exception& e) {
        std::cerr << "Error adding task: " << e.what() << std::endl;
    }
}

void TaskManager::deleteTask(int id) {
    try {
        if (tasks_.count(id)) {
            tasks_.erase(id);
            saveTasks();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error deleting task: " << e.what() << std::endl;
    }
}

const std::optional<std::shared_ptr<Task>> TaskManager::getTask(int id) {
    try {
        auto it = tasks_.find(id);
        if (it != tasks_.end()) {
            return it->second;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting task: " << e.what() << std::endl;
    }
    // Return nullopt for non-existent tasks
    return std::nullopt;
}

const std::optional<std::shared_ptr<Task>> TaskManager::getTaskName(const std::string& name) {
    try {
        for (const auto& pair : tasks_) {
            if (pair.second->getName() == name) {
                return pair.second;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error getting task by name: " << e.what() << std::endl;
    }
    return std::nullopt;
}

bool TaskManager::completeTask(int id) {
    try {
        if (tasks_.count(id)) {
            // Logic to mark task as complete
            tasks_[id]->setIsCompleted(true);
            saveTasks();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error completing task: " << e.what() << std::endl;
    }
    return false;
}

const std::vector<std::shared_ptr<Task>> TaskManager::getPendingTasks() {
    std::vector<std::shared_ptr<Task>> pending;
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted()) { 
            pending.push_back(pair.second);
        }
    }
    return pending;
}

const std::vector<std::shared_ptr<Task>> TaskManager::getTasks() {
    std::vector<std::shared_ptr<Task>> allTasks;
    for (const auto& pair : tasks_) {
        allTasks.push_back(pair.second);
    }
    return allTasks;
}

void TaskManager::listAllTasks() {
    std::cout << "Listing all tasks..." << std::endl;
    for (const auto& task : getTasks()) {
        std::cout << "ID: " << task->getId() << ", Name: " << task->getName() << std::endl;
    }
}

// Helper function to parse due date string to time_point
std::chrono::system_clock::time_point TaskManager::parseDueDate(const std::string& dueDateStr) {
    try{
        std::tm timeVal = {};
        
        // Try parsing YYYY-MM-DD HH:MM:SS format first
        int parsed = std::sscanf(dueDateStr.c_str(), "%d-%d-%d %d:%d:%d",
                                &timeVal.tm_year, &timeVal.tm_mon, &timeVal.tm_mday,
                                &timeVal.tm_hour, &timeVal.tm_min, &timeVal.tm_sec);
        
        // If that fails, try YYYY-MM-DD format only
        if (parsed != 6) {
            parsed = std::sscanf(dueDateStr.c_str(), "%d-%d-%d",
                            &timeVal.tm_year, &timeVal.tm_mon, &timeVal.tm_mday);
        }
        
        if (parsed != 6 && parsed != 3) {
            std::cerr << "Error: Invalid due date format. Expected YYYY-MM-DD or YYYY-MM-DD HH:MM:SS, got: " << dueDateStr << std::endl;
            // Return current time as fallback
            return std::chrono::system_clock::now();
        }
        
        // Set default time values if not provided
        if (parsed == 3) {
            timeVal.tm_hour = 0;
            timeVal.tm_min = 0;
            timeVal.tm_sec = 0;
        }
        
        // FIX: tm_year is years since 1900, so subtract 1900 from the input year
        timeVal.tm_year -= 1900;
        
        // Validate date components (after adjusting tm_year)
        if (timeVal.tm_year < 0 || timeVal.tm_mon < 1 || timeVal.tm_mon > 12 ||
            timeVal.tm_mday < 1 || timeVal.tm_hour > 23 || timeVal.tm_min > 59 || timeVal.tm_sec > 59) {
            std::cerr << "Error: Invalid date values." << std::endl;
            return std::chrono::system_clock::now();
        }
        
        // Convert to time_t
        time_t currentTime = std::mktime(&timeVal);
        
        // Check if mktime succeeded
        if (currentTime == (-1)) {
            std::cerr << "Error: Failed to convert date to time_t." << std::endl;
            return std::chrono::system_clock::now();
        }
        
        // Convert to time_point
        return std::chrono::system_clock::from_time_t(currentTime);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing due date: " << e.what() << std::endl;
        return std::chrono::system_clock::now();
    }
}