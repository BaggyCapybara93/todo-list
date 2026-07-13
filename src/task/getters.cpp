#include "manager.hpp"
#include <iostream>
#include "settings.hpp"
#include "logger.hpp"
#include "utils/utils.hpp"


//Task Management
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

std::shared_ptr<Tag> TaskManager::getTagById(const int& id) {
    return tagManager_->getTagById(id);
}

std::shared_ptr<Tag> TaskManager::getTagByName(const std::string& name) {
    return tagManager_->getTagByName(name);
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

RepeatInterval TaskManager::getRepeatInterval(int taskId) {
    auto taskOpt = getTask(taskId);
    if (taskOpt.has_value()) {
        return taskOpt.value()->getRepeatInterval();
    }
    return RepeatInterval::NEVER;
}

//Subtasks and Dependencies
std::unordered_set<std::shared_ptr<Task>> TaskManager::getSubtasks(int taskId) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getSubtasks();
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving subtasks for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return {};
}

std::shared_ptr<Task> TaskManager::getSubtaskById(int taskId, int subtaskId) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getSubtaskById(subtaskId);
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving subtask by id for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return nullptr;
}

std::shared_ptr<Task> TaskManager::getSubtaskByName(int taskId, const std::string& subtaskName) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getSubtaskByName(subtaskName);
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving subtask by name for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return nullptr;
}

std::unordered_set<std::shared_ptr<Task>> TaskManager::getDependencies(int taskId) {
    try {
        auto taskOpt = TaskManager::getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getDependencies();
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving dependencies for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return {};
}

std::shared_ptr<Task> TaskManager::getDependencyById(int taskId, int dependencyId) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getDependencyById(dependencyId);
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving dependency by id for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return nullptr;
}

std::shared_ptr<Task> TaskManager::getDependencyByName(int taskId, const std::string& dependencyName) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getDependencyByName(dependencyName);
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving dependency by name for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return nullptr;
}