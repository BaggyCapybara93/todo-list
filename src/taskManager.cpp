#include "taskManager.hpp"
#include <iostream>
#include "settings.hpp"
#include "utils.hpp"

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
