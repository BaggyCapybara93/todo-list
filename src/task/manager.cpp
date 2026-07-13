#include "task/manager.hpp"
#include <iostream>
#include "settings.hpp"
#include "logger.hpp"
#include "utils/utils.hpp"

// Constructor implementation
TaskManager::TaskManager(std::shared_ptr<FileManager> fm, std::shared_ptr<TagManager> tm, std::shared_ptr<Settings> s)
    : nextId_(1), fileManager_(fm), tagManager_(tm), settings_(s) {
    loadTasks();
}

// Destructor to save tasks if autosave is enabled
TaskManager::~TaskManager() {
    try {
        if (settings_.get()->getEnableAutosave()) {
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
            int maxId = 0;
            for (const auto& task : *tasksOpt) {
                if (settings_.get()->getMaxTasksPerFile() > 0 && tasks_.size() >= static_cast<size_t>(settings_.get()->getMaxTasksPerFile())) {
                    Logger::log(Logger::LogLevel::WARNING, "Maximum tasks per file limit reached. Some tasks may not be loaded.");
                    break;
                }

                if (getTask(task->getId()).has_value()){
                    Logger::log(Logger::LogLevel::WARNING, "Duplicate task ID: " + std::to_string(task->getId()) + " found in file. Skipping.");
                    continue;
                }

                tasks_[task->getId()] = task;
                // Update maxId to track the highest task ID
                if (task->getId() > maxId) {
                    maxId = task->getId();
                }
            }
            // Set nextId_ to maxId + 1 to avoid conflicts
            nextId_ = maxId + 1;
            if (settings_.get()->getVerbose()) {
                std::cout << "Tasks loaded successfully." << std::endl;
            }
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
        if (fileManager_.get()->saveTodoList(tasksToSave) && settings_.get()->getVerbose()) {
            std::cout << "Tasks saved successfully." << std::endl;
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
    }
}

int TaskManager::addTask(const std::string& name, const std::string& description, int priority, const std::chrono::system_clock::time_point& dueDate) {
    try{
        int taskId = nextId_++;

        if(priority < 0 || priority > 10) {
            Logger::log(Logger::LogLevel::WARNING, "Priority must be between 0 and 10.");
            return -1;
        }

        auto newTask = std::make_shared<Task>(taskId, name, description, false, priority, dueDate);
        tasks_[taskId] = newTask;
        saveTasks();
        return taskId;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, std::string(e.what()));
        return -1;
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
            if (tasks_[id]->getIsCompleted()) {
                Logger::log(Logger::LogLevel::INFO, "Task already completed: " + std::to_string(id));
                return false;
            }

            if(hasDependency(id)) {
                for (const auto& dep : tasks_[id]->getDependencies()) {
                    if (!dep->getIsCompleted()) {
                        Logger::log(Logger::LogLevel::WARNING, "Cannot complete task " + std::to_string(id) + " because dependency " + std::to_string(dep->getId()) + " is not completed.");
                        return false;
                    }
                }
            }

            if(hasSubtask(id)) {
                for (const auto& subtask : tasks_[id]->getSubtasks()) {
                    if (!subtask->getIsCompleted()) {
                        Logger::log(Logger::LogLevel::WARNING, "Cannot complete task " + std::to_string(id) + " because subtask " + std::to_string(subtask->getId()) + " is not completed.");
                        return false;
                    }
                }
            }

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

// Repeat task methods
bool TaskManager::setRepeatTask(int taskId, const std::string& intervalStr) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return false;
        }

        auto& task = *taskOpt.value();
        task.setRepeatInterval(parseRepeatInterval(intervalStr));
        task.calculateNextDueDate();
        saveTasks();
        return true;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error setting repeat task: " + std::string(e.what()));
        return false;
    }
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

void TaskManager::addSubtask(int taskId, const std::shared_ptr<Task>& subtask) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return;
        }

        if (!subtask) {
            Logger::log(Logger::LogLevel::WARNING, "Subtask is null for task " + std::to_string(taskId));
            return;
        }

        auto task = taskOpt.value();
        task->addSubtask(subtask);
        saveTasks();
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error adding subtask to task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
}

void TaskManager::removeSubtask(int taskId, const std::shared_ptr<Task>& subtask) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return;
        }

        if (!subtask) {
            Logger::log(Logger::LogLevel::WARNING, "Subtask is null for task " + std::to_string(taskId));
            return;
        }

        auto task = taskOpt.value();
        task->removeSubtask(subtask);
        saveTasks();
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error removing subtask from task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
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

bool TaskManager::hasSubtask(int taskId) {
    return !getSubtasks(taskId).empty();
}

std::unordered_set<std::shared_ptr<Task>> TaskManager::getDependencies(int taskId) {
    try {
        auto taskOpt = getTask(taskId);
        if (taskOpt.has_value()) {
            return taskOpt.value()->getDependencies();
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error retrieving dependencies for task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
    return {};
}

void TaskManager::addDependency(int taskId, const std::shared_ptr<Task>& dependency) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return;
        }

        if (!dependency) {
            Logger::log(Logger::LogLevel::WARNING, "Dependency is null for task " + std::to_string(taskId));
            return;
        }

        auto task = taskOpt.value();
        task->addDependency(dependency);
        saveTasks();
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error adding dependency to task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
}

void TaskManager::removeDependency(int taskId, const std::shared_ptr<Task>& dependency) {
    try {
        auto taskOpt = getTask(taskId);
        if (!taskOpt.has_value()) {
            Logger::log(Logger::LogLevel::WARNING, "Task not found: " + std::to_string(taskId));
            return;
        }

        if (!dependency) {
            Logger::log(Logger::LogLevel::WARNING, "Dependency is null for task " + std::to_string(taskId));
            return;
        }

        auto task = taskOpt.value();
        task->removeDependency(dependency);
        saveTasks();
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error removing dependency from task " + std::to_string(taskId) + ": " + std::string(e.what()));
    }
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

bool TaskManager::hasDependency(int taskId) {
    if (tasks_.count(taskId)) {
        return !tasks_[taskId]->getDependencies().empty();
    }
    return false;
}

void TaskManager::calculateNextDueDates() {
    for (auto& pair : tasks_) {
        pair.second->calculateNextDueDate();
    }
    saveTasks();
}
