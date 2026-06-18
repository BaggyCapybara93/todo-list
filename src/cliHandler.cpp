#include "cliHandler.hpp"
#include <iostream>
#include <string>
#include "utils.hpp"
#include "logger.hpp"
#include "settings.hpp"


CLIHandler::CLIHandler(TaskManager& tm, FileManager& fm, po::variables_map config)
    : taskManager_(tm), fileManager_(fm), config_(config) {
}

void CLIHandler::handleAddTask() {
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

        taskManager_.addTask(name, description, priority, dueDate);
        std::cout << "Task added successfully.\n";
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error adding task: " + std::string(e.what()));
    }
}

void CLIHandler::handleListTasks() {
    std::vector<std::shared_ptr<Task>> tasksToDisplay;
    
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
        
        tasksToDisplay = taskManager_.filterTasksByDueDateAndPriority(dueDateMin, dueDateMax, priorityMin, priorityMax);
    } else {
        // Default: show all pending tasks
        std::cout << "\n--- Pending Tasks ---\n";
        tasksToDisplay = taskManager_.getPendingTasks();
    }
    
    if (tasksToDisplay.empty()) {
        std::cout << "No pending tasks found.\n";
    } else {
        for (const auto& task : tasksToDisplay) {
            std::cout << task->toString() << std::endl;
        }
    }
}

void CLIHandler::handleCompleteTask() {
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
        
        if (taskManager_.completeTask(id)) {
            std::cout << "Task ID " << id << " marked as complete.\n";
        } else {
            std::cout << "Error: Task ID " << id << " not found.\n";
        }
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Unknown error completing task: " + std::string(e.what()));
    }
}

void CLIHandler::handleFileOperations() {
    try{
        // Check if CLI option is provided
        if (config_.count("export") > 0) {
            // Export via CLI
            std::cout << "Exporting tasks...\n";
            if (fileManager_.saveTodoList(taskManager_.getTasks())) {
                std::cout << "Tasks exported successfully to " << fileManager_.getTodoFilePath() << "\n";
            } else {
                std::cerr << "Failed to export tasks.\n";
            }
        } else if (config_.count("import") > 0) {
            // Import via CLI
            std::cout << "Importing tasks...\n";
            if (fileManager_.loadTodoList()) {
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

void CLIHandler::handleHelp() {
    CLIHandler::displayHelp(config_);
}

void CLIHandler::handleExit() {
    Logger::log(Logger::LogLevel::INFO, "Exiting...");
    exit(0);
}

void CLIHandler::execute() {
    try{
        if (config_.count("help") > 0) {
            handleHelp();
        } else if (config_.count("add") > 0) {
            handleAddTask();
        } else if (config_.count("list") > 0) {
            handleListTasks();
        } else if (config_.count("complete") > 0) {
            handleCompleteTask();
        } else if (config_.count("export") > 0 || config_.count("import") > 0) {
            handleFileOperations();
        } else if (config_.count("exit") > 0) {
            handleExit();
        } else {
            Logger::log(Logger::LogLevel::ERROR, "No command specified.");
            Logger::log(Logger::LogLevel::INFO, "Use --help for more information.");
            
            handleExit();
        }
    } catch(const std::exception& e){
        Logger::log(Logger::LogLevel::ERROR, "Unknown error occured while handling command: "+ std::string(e.what()));
    }
}

void CLIHandler::displayHelp(const po::variables_map& vm) {
    std::cout << "\nTodo List Application - Help\n";
    std::cout << "=============================\n\n";
    
    if (vm.count("help") > 0) {
        std::cout << "General Help:\n";
        std::cout << "  This application manages todo lists with the following commands:\n\n";
        std::cout << "  --help, -h          Show this help message\n";
        std::cout << "  --menu, -m          Run in interactive menu mode\n";
        std::cout << "  --add, -a           Add a new task\n";
        std::cout << "  --list, -l          List all tasks\n";
        std::cout << "  --complete, -c      Complete a task\n";
        std::cout << "  --export, -e        Export tasks to file\n";
        std::cout << "  --import, -i        Import tasks from file\n";
    }
    else if (vm.count("add") > 0) {
        std::cout << "Add Task Help:\n";
        std::cout << "  Add a new task to the todo list.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --name, -n          Task name (required)\n";
        std::cout << "    --description, -d   Task description (required)\n";
        std::cout << "    --due-date, -u      Due date in YYYY-MM-DD HH:MM:SS format (required)\n\n";
        std::cout << "  Optional options:\n";
        std::cout << "    --priority, -p      Priority level (0-10), default: 5\n";
    }
    else if (vm.count("list") > 0) {
        std::cout << "List Tasks Help:\n";
        std::cout << "  List all tasks in the todo list.\n\n";
        std::cout << "  Options:\n";
        std::cout << "    Without filters: Shows all pending tasks\n\n";
        std::cout << "    With filters: Use the following options to filter pending tasks:\n";
        std::cout << "      --due-date-min, -dm    Minimum due date (YYYY-MM-DD HH:MM:SS)\n";
        std::cout << "      --due-date-max, -dM    Maximum due date (YYYY-MM-DD HH:MM:SS)\n";
        std::cout << "      --priority-min, -pm    Minimum priority (0-10)\n";
        std::cout << "      --priority-max, -pM    Maximum priority (0-10)\n";
        std::cout << "      Example: --list --due-date-min 2024-01-01 00:00:00 --priority-min 5\n";
    }
    else if (vm.count("complete") > 0) {
        std::cout << "Complete Task Help:\n";
        std::cout << "  Mark a task as complete.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --task-id, -T       Task ID to complete (required)\n";
    }
    else if (vm.count("export") > 0) {
        std::cout << "Export Tasks Help:\n";
        std::cout << "  Export all tasks to the todo list file.\n\n";
        std::cout << "  This saves all current tasks to " << fileManager_.getTodoFilePath() << "\n";
    }
    else if (vm.count("import") > 0) {
        std::cout << "Import Tasks Help:\n";
        std::cout << "  Import tasks from the todo list file.\n\n";
        std::cout << "  This loads tasks from " << fileManager_.getTodoFilePath() << "\n";
    }
    else {
        std::cout << "General Help:\n";
        std::cout << "  This application manages todo lists with the following commands:\n\n";
        std::cout << "  --help, -h          Show this help message\n";
        std::cout << "  --menu, -m          Run in interactive menu mode\n";
        std::cout << "  --add, -a           Add a new task\n";
        std::cout << "  --list, -l          List all tasks\n";
        std::cout << "  --complete, -c      Complete a task\n";
        std::cout << "  --export, -e        Export tasks to file\n";
        std::cout << "  --import, -i        Import tasks from file\n";
    }
}




