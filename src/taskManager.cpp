#include "taskManager.hpp"
#include <iostream>
#include "settings.hpp"
#include "logger.hpp"
#include "utils.hpp"

// Constructor implementation
TaskManager::TaskManager(std::shared_ptr<FileManager> fm, std::shared_ptr<TagManager> tm) : nextId_(1), fileManager_(fm), tagManager_(tm) {
    loadTasks();
}

// Destructor to save tasks if autosave is enabled
TaskManager::~TaskManager() {
    try {
        if (setting_.enableAutosave) {
            saveTasks();
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Destructor error: " + std::string(e.what()));
    }
}

void TaskManager::loadTasks() {
    try{
        auto tasksOpt = fileManager_.get()->loadTodoList();
        if (tasksOpt) {
            tasks_.clear();
            for (const auto& task : *tasksOpt) {
                if (setting_.maxTasksPerFile > 0 && tasks_.size() >= static_cast<size_t>(setting_.maxTasksPerFile)) {
                    Logger::log(Logger::LogLevel::WARNING, "Maximum tasks per file limit reached. Some tasks may not be loaded.");
                    break;
                }

                if (getTask(task->getId()).has_value()){
                    Logger::log(Logger::LogLevel::WARNING, "Duplicate task ID: " + std::to_string(task->getId()) + " found in file. Skipping.");
                    continue;
                }

                tasks_[task->getId()] = task;
            }
            std::cout << "Tasks loaded successfully." << std::endl;
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Could not load tasks, starting with empty list.");
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
}

void TaskManager::saveTasks() {
    try{
        std::vector<std::shared_ptr<Task>> tasksToSave;
        for (const auto& pair : tasks_) {
            tasksToSave.push_back(pair.second);
        }
        if (fileManager_.get()->saveTodoList(tasksToSave)) {
            std::cout << "Tasks saved successfully." << std::endl;
        } else {
            Logger::log(Logger::LogLevel::ERROR, "Failed to save tasks");
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
}

void TaskManager::addTask(const std::string& name, const std::string& description, int priority, const std::chrono::system_clock::time_point& dueDate) {
    try{
        int taskId = nextId_++;

        if(priority < 0 || priority > 10) {
            Logger::log(Logger::LogLevel::WARNING, "Priority must be between 0 and 10.");
            return;
        }

        auto newTask = std::make_shared<Task>(taskId, name, description, false, priority, dueDate);
        tasks_[taskId] = newTask;
        saveTasks();
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
}

void TaskManager::deleteTask(int id) {
    try {
        if (tasks_.count(id)) {
            tasks_.erase(id);
            saveTasks();
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
}

const std::optional<std::shared_ptr<Task>> TaskManager::getTask(int id) {
    try {
        auto it = tasks_.find(id);
        if (it != tasks_.end()) {
            return it->second;
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
    // Return nullopt for non-existent tasks
    return std::nullopt;
}

bool TaskManager::removeTag(const std::string& name, const int& id) {
    return tagManager_->removeTag(name, id);
}

const std::optional<std::shared_ptr<Task>> TaskManager::getTaskName(const std::string& name) {
    try {
        for (const auto& pair : tasks_) {
            if (pair.second->getName() == name) {
                return pair.second;
            }
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
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
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
    return false;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::getPendingTasks() {
    auto pending = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted()) { 
            pending->push_back(pair.second);
        }
    }
    return pending;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::getTasks() {
    auto allTasks = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        allTasks->push_back(pair.second);
    }
    return allTasks;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByDueDate(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getDueDate() != std::chrono::system_clock::time_point()) {
            if (pair.second->getDueDate() >= minDueDate && pair.second->getDueDate() <= maxDueDate) {
                filtered->push_back(pair.second);
            }
        }
    }
    return filtered;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByPriority(int minPriority, int maxPriority) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getPriority() >= minPriority && pair.second->getPriority() <= maxPriority) {
            filtered->push_back(pair.second);
        }
    }
    return filtered;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByDueDateAndPriority(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate, int minPriority, int maxPriority) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getDueDate() != std::chrono::system_clock::time_point()) {
            if (pair.second->getDueDate() >= minDueDate && pair.second->getDueDate() <= maxDueDate &&
                pair.second->getPriority() >= minPriority && pair.second->getPriority() <= maxPriority) {
                filtered->push_back(pair.second);
            }
        }
    }
    return filtered;
}

void TaskManager::listAllTasks() {
    std::cout << "Listing all tasks..." << std::endl;
    for (const auto& task : *getTasks().get()) {
        std::cout << "ID: " << task->getId() << ", Name: " << task->getName() << std::endl;
    }
}

// Tag-related implementations
bool TaskManager::addTag(const std::string& name, const std::string& description) {
    return tagManager_->addTag(name, description);
}

std::shared_ptr<Tag> TaskManager::getTagById(const int& id) {
    return tagManager_->getTagById(id);
}

std::shared_ptr<Tag> TaskManager::getTagByName(const std::string& name) {
    return tagManager_->getTagByName(name);
}

bool TaskManager::addTagToTask(int taskId, const std::string& tagName) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return false;
        }

        auto tag = tagManager_->getTagByName(tagName);
        if (!tag) {
            Logger::log(Logger::LogLevel::WARNING, "Tag not found: " + tagName);
            return false;
        }

        // Check if tag is already added to this task
        auto& task = *taskOpt.value();
        for (const auto& existingTag : task.getTags()) {
            if (existingTag->getName() == tagName) {
                return true; // Tag already exists
            }
        }

        task.addTag(tag);
        saveTasks();
        return true;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error adding tag to task: " + std::string(e.what()));
        return false;
    }
}

bool TaskManager::removeTagFromTask(int taskId, const std::string& tagName) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return false;
        }

        auto tag = tagManager_->getTagByName(tagName);
        if (!tag) {
            Logger::log(Logger::LogLevel::WARNING, "Tag not found: " + tagName);
            return false;
        }

        auto& task = *taskOpt.value();
        task.removeTag(tag);
        saveTasks();
        return true;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error removing tag from task: " + std::string(e.what()));
        return false;
    }
}
