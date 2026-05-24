#include "cliHandler.hpp"
#include <iostream>
#include <string>
#include "dateUtils.hpp"
#include "settings.hpp"


CLIHandler::CLIHandler(TaskManager& tm, FileManager& fm, po::variables_map config)
    : taskManager_(tm), fileManager_(fm), config_(config) {
}

void CLIHandler::handleAddTask() {
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
        std::cerr << "Error: Missing required arguments for add task.\n";
        std::cerr << "Use --name, --description, and --due-date flags.\n";
        return;
    }

    // Validate priority range
    if (priority < 0 || priority > 10) {
        std::cerr << "Error: Priority must be between 0 and 10." << std::endl;
        return;
    }
    
    // Convert dueDateStr to time_point
    auto dueDate = parseDueDate(dueDateStr);

    taskManager_.addTask(name, description, priority, dueDate);
    std::cout << "Task added successfully.\n";
}

void CLIHandler::handleListTasks() {
    std::cout << "\n--- Pending Tasks ---\n";
    auto pendingTasks = taskManager_.getPendingTasks();
    if (pendingTasks.empty()) {
        std::cout << "No pending tasks found.\n";
    } else {
        for (const auto& task : pendingTasks) {
            std::cout << task->toString() << std::endl;
        }
    }
}

void CLIHandler::handleCompleteTask() {
    int id;
    
    // Check if CLI option is provided
    if (config_.count("task-id") > 0) {
        // Use CLI option
        id = config_["task-id"].as<int>();
    } else {
        std::cerr << "Error: Missing required argument for complete task.\n";
        std::cerr << "Use --task-id flag.\n";
        return;
    }
    
    if (taskManager_.completeTask(id)) {
        std::cout << "Task ID " << id << " marked as complete.\n";
    } else {
        std::cout << "Error: Task ID " << id << " not found.\n";
    }
}

void CLIHandler::handleFileOperations() {
    // Check if CLI option is provided
    if (config_.count("export") > 0) {
        // Export via CLI
        std::cout << "Exporting tasks...\n";
        if (fileManager_.saveTodoList(taskManager_.getTasks())) {
            std::cout << "Tasks exported successfully to " << fileManager_.getTodoFilePath() << "\n";
        } else {
            std::cerr << "Error: Failed to export tasks.\n";
        }
    } else if (config_.count("import") > 0) {
        // Import via CLI
        std::cout << "Importing tasks...\n";
        if (fileManager_.loadTodoList()) {
            std::cout << "Tasks imported successfully.\n";
        } else {
            std::cout << "Error: Failed to import tasks.\n";
        }
    } else {
        std::cerr << "Error: Missing required argument for file operations.\n";
        std::cerr << "Use --export or --import flag.\n";
    }
}

void CLIHandler::handleHelp() {
    CLIHandler::displayHelp(config_);
}

void CLIHandler::handleExit() {
    std::cout << "Exiting application. Goodbye!\n";
    exit(0);
}

void CLIHandler::execute() {
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
        std::cerr << "Error: No command specified.\n";
        displayHelp(config_);
        std::cerr << "Use --help for more information.\n";
        std::cerr << "\nExiting application.\n";
        exit(1);
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
        std::cout << "  This will display all pending and completed tasks.\n";
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




