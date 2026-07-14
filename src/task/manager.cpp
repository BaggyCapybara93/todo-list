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

bool TaskManager::removeTag(const std::string& name, const int& id) {
    return tagManager_->removeTag(name, id);
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

bool TaskManager::hasSubtask(int taskId) {
    return !getSubtasks(taskId).empty();
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
