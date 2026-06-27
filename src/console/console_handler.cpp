#include "console.hpp"

void Console::handleAddTask() {
    std::cout << "--- Add Task ---\n";
    std::string name, description, dueDateStr;
    int priority;
    std::cout << "Enter task name: ";
    std::getline(std::cin, name);
    std::cout << "Enter task description: ";
    std::getline(std::cin, description);
    std::cout << "Enter priority (0-10): ";
    std::cin >> priority;
    std::cin.ignore(); 
    std::cout << "Enter due date (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, dueDateStr);

    // Validate priority range
    if (priority < 0 || priority > 10) {
        std::cerr << "Error: Priority must be between 0 and 10." << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }
    
    // Convert dueDateStr to time_point
    auto dueDate = parseDueDate(dueDateStr);
    
    taskManager_.get()->addTask(name, description, priority, dueDate);
    std::cout << "Task added successfully.\n";
}

void Console::handleListTasks() {
    std::cout << "\n--- Pending Tasks ---\n";
    auto pendingTasks = taskManager_.get()->getPendingTasks();
    if (pendingTasks.get()->empty()) {
        std::cout << "No pending tasks found.\n";
    } else {
        for (const auto& task : *pendingTasks.get()) {
            std::cout << task.get()->toString() << std::endl;
        }
    }
}

void Console::handleCompleteTask() {
    std::cout << "\n--- Complete Task ---\n";
    std::cout << "Enter Task ID to complete: ";
    int id;
    std::cin >> id;
    
    // Validate task ID
    if (id <= 0) {
        std::cerr << "Error: Task ID must be a positive number." << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }
    
    std::cin.ignore(); // Clear buffer
    
    if (taskManager_.get()->completeTask(id)) {
        std::cout << "Task ID " << id << " marked as complete.\n";
    } else {
        std::cout << "Error: Task ID " << id << " not found.\n";
    }
}

void Console::handleAddTag() {
    std::cout << "\n--- Add Tag ---\n";
    std::string name, description;
    std::cout << "Enter tag name: ";
    std::getline(std::cin, name);
    std::cout << "Enter tag description (optional): ";
    std::getline(std::cin, description);
    
    if (taskManager_.get()->addTag(name, description)) {
        std::cout << "Tag '" << name << "' added successfully.\n";
    } else {
        std::cout << "Failed to add tag.\n";
    }
}

void Console::handleRemoveTag() {
    std::cout << "\n--- Remove Tag ---\n";
    std::cout << "Enter tag name or ID to remove: ";
    std::string input;
    std::getline(std::cin, input);
    
    int tagId = -1;
    std::string tagName = input;
    
    // Check if input is a number
    try {
        tagId = std::stoi(input);
    } catch (...) {
        // Not a number, treat as tag name
        tagId = -1;
    }
    
    if (taskManager_.get()->removeTag(tagName, tagId)) {
        std::cout << "Tag removed successfully.\n";
    }
}

void Console::handleSetRepeatTask() {
    std::cout << "\n--- Set Repeat Task ---\n";
    std::cout << "Enter Task ID: ";
    int taskId;
    std::cin >> taskId;
    std::cin.ignore();
    
    // Validate task ID
    if (taskId <= 0) {
        std::cerr << "Error: Task ID must be a positive number." << std::endl;
        return;
    }
    
    std::cout << "Enter repeat interval (NEVER, DAILY, WEEKLY, MONTHLY, YEARLY): ";
    std::string intervalStr;
    std::getline(std::cin, intervalStr);
    
    if (intervalStr.empty()) {
        std::cerr << "Error: Repeat interval cannot be empty." << std::endl;
        return;
    }
    
    if (taskManager_.get()->setRepeatTask(taskId, intervalStr)) {
        auto task = taskManager_.get()->getTask(taskId);
        if (task.has_value()) {
            std::cout << "Repeat interval set successfully for task ID " << taskId << ".\n";
            std::cout << "Next due date: " << task->get()->getNextDueDateStr() << "\n";
        }
    } else {
        std::cout << "Error: Failed to set repeat interval.\n";
    }
}

void Console::handleExit(){
    std::cout << "Exiting application. Goodbye!\n";
    exit(0);
}

void Console::displayMenu() {
    std::cout << "\n========================\n";
    std::cout << "       Todo List App\n";
    std::cout << "========================\n";
    std::cout << "1. Add Task\n";
    std::cout << "2. List Tasks\n";
    std::cout << "3. Complete Task\n";
    std::cout << "4. Add Tag\n";
    std::cout << "5. Remove Tag\n";
    std::cout << "6. Set Repeat Task\n";
    std::cout << "8. File Operations\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter your choice: ";
}

void Console::handleFileOperations() {
    std::cout << "\n--- File Operations ---\n";
    std::cout << "1. Export Tasks to File\n";
    std::cout << "2. Import Tasks from File\n";
    std::cout << "3. Back to Main Menu\n";
    std::cout << "Enter your choice: ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Exporting tasks...\n";
        // Export tasks using FileManager's saveTodoList method
        auto tasks = taskManager_.get()->getTasks();
        if (tasks && !tasks->empty()) {
            if (fileManager_.get()->saveTodoList(*tasks)) {
                std::cout << "Tasks exported successfully to " << fileManager_.get()->getTodoFilePath() << "\n";
            } else {
                std::cerr << "Error: Failed to export tasks.\n";
            }
        } else {
            std::cout << "No tasks to export.\n";
        }
    } else if (choice == "2") {
        std::cout << "Importing tasks...\n";
        // Assuming FileManager has an import method
        if (fileManager_.get()->loadTodoList()) {
            std::cout << "Tasks imported successfully.\n";
        } else {
            std::cout << "Error: Failed to import tasks.\n";
        }
    } else if (choice == "3") {
        // Do nothing, just return to the main menu loop
    } else {
        std::cout << "Invalid file operation choice.\n";
    }
}

void Console::handleInput(std::string input) {
    if (commandMap_.count(input)) {
        commandMap_[input]();
    } else {
        std::cout << "Invalid command. Please enter a number between 1 and 6.\n";
    }
}