#include "cli.hpp"

void CLI::handleAddTask() {
    try {
        std::string name, description, dueDateStr;
        int priority;
        
        // Check if CLI options are provided
        if (config_.count("name") > 0 && config_.count("description") > 0 && config_.count("due-date") > 0) {
            // Use CLI options
            name = config_["name"].as<std::string>();
            description = config_["description"].as<std::string>();
            dueDateStr = config_["due-date"].as<std::string>();
            priority = config_.count("priority") > 0 ? config_["priority"].as<int>() : 5;
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Missing required arguments for add task.");
            Logger::log(Logger::LogLevel::INFO, "Use --name, --description, and --due-date flags."); //Change these to not be loggged in the file or as an additional output for this

            return;
        }

        // Validate priority range
        if (priority < 0 || priority > 10) {
            Logger::log(Logger::LogLevel::WARNING, "Priority must be between 0 and 10");
            return;
        }
        
        // Convert dueDateStr to time_point
        auto dueDate = parseDueDate(dueDateStr);

        taskManager_.get()->addTask(name, description, priority, dueDate);
        std::cout << "Task added successfully.\n";
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error adding task: " + std::string(e.what()));
    }
}

void CLI::handleListTasks() {
    try{
        std::shared_ptr<std::vector<std::shared_ptr<Task>>> tasksToDisplay;
        
        // Check if filtering options are provided
        bool hasDueDateMin = config_.count("due-date-min") > 0;
        bool hasDueDateMax = config_.count("due-date-max") > 0;
        bool hasPriorityMin = config_.count("priority-min") > 0;
        bool hasPriorityMax = config_.count("priority-max") > 0;
        
        if (hasDueDateMin || hasDueDateMax || hasPriorityMin || hasPriorityMax) {
            // Filtered list
            std::cout << "\n--- Filtered Pending Tasks ---\n";
            
            auto dueDateMin = hasDueDateMin ? parseDueDate(config_["due-date-min"].as<std::string>()) : std::chrono::system_clock::time_point();
            auto dueDateMax = hasDueDateMax ? parseDueDate(config_["due-date-max"].as<std::string>()) : std::chrono::system_clock::time_point::max();
            int priorityMin = hasPriorityMin ? config_["priority-min"].as<int>() : 0;
            int priorityMax = hasPriorityMax ? config_["priority-max"].as<int>() : 10;
            
            // Validate priority range
            if (priorityMin < 0 || priorityMin > 10 || priorityMax < 0 || priorityMax > 10 || priorityMin > priorityMax) {
                Logger::log(Logger::LogLevel::WARNING, "Priority must be between 0 and 10, and min must be <= max.");
                return;
            }
            
            tasksToDisplay = taskManager_.get()->filterTasksByDueDateAndPriority(dueDateMin, dueDateMax, priorityMin, priorityMax);
        } else {
            // Default: show all pending tasks
            std::cout << "\n--- Pending Tasks ---\n";
            tasksToDisplay = taskManager_.get()->getPendingTasks();
        }
        
        if (tasksToDisplay->empty()) {
            std::cout << "No pending tasks found.\n";
        } else {
            for (const auto& task : *tasksToDisplay) {
                std::cout << task->toString() << std::endl;
            }
        }
    }catch(const std::exception& e){
        Logger::log(Logger::LogLevel::ERROR, "Unknown error occurred while listing tasks: " + std::string(e.what()));
    }
}

void CLI::handleCompleteTask() {
    try {
        int id;
        
        // Check if CLI option is provided
        if (config_.count("task-id") > 0) {
            // Use CLI option
            id = config_["task-id"].as<int>();
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Missing required argument for complete task.");
            Logger::log(Logger::LogLevel::INFO, "Use --task-id flag.");
            return;
        }
        
        if (taskManager_.get()->completeTask(id)) {
            std::cout << "Task ID " << id << " marked as complete.\n";
        } else {
            std::cout << "Error: Task ID " << id << " not found.\n";
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error completing task: " + std::string(e.what()));
    }
}

void CLI::handleFileOperations() {
    try{
        // Check if CLI option is provided
        if (config_.count("export") > 0) {
            // Export via CLI
            std::cout << "Exporting tasks...\n";
            auto tasks = taskManager_.get()->getTasks();
            if (tasks && !tasks->empty()) {
                if (fileManager_.get()->saveTodoList(*tasks)) {
                    std::cout << "Tasks exported successfully to " << fileManager_.get()->getTodoFilePath() << "\n";
                } else {
                    std::cerr << "Failed to export tasks.\n";
                }
            }
        } else if (config_.count("import") > 0) {
            // Import via CLI
            std::cout << "Importing tasks...\n";
            if (fileManager_.get()->loadTodoList()) {
                std::cout << "Tasks imported successfully.\n";
            } else {
                Logger::log(Logger::LogLevel::ERROR, "Failed to import tasks.");
            }
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Missing required argument for file operations.");
            Logger::log(Logger::LogLevel::INFO, "Use --export or --import flag.");
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error handling file operations: " + std::string(e.what()));
    }
}

void CLI::handleHelp() {
    CLI::displayHelp(config_);
}

void CLI::handleExit() {
    Logger::log(Logger::LogLevel::INFO, "Exiting...");
    exit(0);
}

void CLI::handleAddTag() {
    try {
        std::string name, description;
        
        if (config_.count("tag-name") > 0 && config_.count("tag-description") > 0) {
            name = config_["tag-name"].as<std::string>();
            description = config_["tag-description"].as<std::string>();
            
            if (taskManager_.get()->addTag(name, description)) {
                std::cout << "Tag '" << name << "' added successfully.\n";
            } else {
                std::cout << "Failed to add tag.\n";
            }
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Missing required arguments for add tag.");
            Logger::log(Logger::LogLevel::INFO, "Use --tag-name and --tag-description flags.");
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error adding tag: " + std::string(e.what()));
    }
}

void CLI::handleRemoveTag() {
    try {
        std::string tagName;
        int tagId = -1;
        
        if (config_.count("tag-name") > 0) {
            tagName = config_["tag-name"].as<std::string>();
        } else if (config_.count("tag-id") > 0) {
            tagId = config_["tag-id"].as<int>();
        } else {
            Logger::log(Logger::LogLevel::WARNING, "Missing required arguments for remove tag.");
            Logger::log(Logger::LogLevel::INFO, "Use --tag-name or --tag-id flag.");
            return;
        }
        
        if (taskManager_.get()->removeTag(tagName, tagId)) {
            std::cout << "Tag removed successfully.\n";
        } else {
            std::cout << "Failed to remove tag.\n";
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error removing tag: " + std::string(e.what()));
    }
}