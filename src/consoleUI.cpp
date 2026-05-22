#include "consoleUI.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include "settings.hpp"

// Constructor implementation
ConsoleUI::ConsoleUI(TaskManager& tm, FileManager& fm) : taskManager_(tm), fileManager_(fm) {
    initializeCommands();
}

// Initialize the command map
void ConsoleUI::initializeCommands() {
    commandMap_["1"] = [this]() { this->handleAddTask(); };
    commandMap_["2"] = [this]() { this->handleListTasks(); };
    commandMap_["3"] = [this]() { this->handleCompleteTask(); };
    commandMap_["4"] = [this]() { this->handleFileOperations(); };
    commandMap_["5"] = [this]() { this->handleExit(); };
}

// --- Command Handlers ---

void ConsoleUI::handleAddTask() {
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
    
    taskManager_.addTask(name, description, priority, dueDate);
    std::cout << "Task added successfully.\n";
}

void ConsoleUI::handleListTasks() {
    std::cout << "\n--- Pending Tasks ---\n";
    auto pendingTasks = taskManager_.getPendingTasks();
    if (pendingTasks.empty()) {
        std::cout << "No pending tasks found.\n";
    } else {
        for (const auto& task : pendingTasks) {
            std::cout << task.get()->toString() << std::endl;
        }
    }
}

void ConsoleUI::handleCompleteTask() {
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
    
    if (taskManager_.completeTask(id)) {
        std::cout << "Task ID " << id << " marked as complete.\n";
    } else {
        std::cout << "Error: Task ID " << id << " not found.\n";
    }
}

void ConsoleUI::handleExit(){
    std::cout << "Exiting application. Goodbye!\n";
    exit(0);
}

void ConsoleUI::displayMenu() {
    std::cout << "\n========================\n";
    std::cout << "       Todo List App\n";
    std::cout << "========================\n";
    std::cout << "1. Add Task\n";
    std::cout << "2. List Tasks\n";
    std::cout << "3. Complete Task\n";
    std::cout << "4. File Operations\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter your choice: ";
}

void ConsoleUI::handleFileOperations() {
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
        if (fileManager_.saveTodoList(taskManager_.getTasks())) {
            std::cout << "Tasks exported successfully to " << fileManager_.getTodoFilePath() << "\n";
        } else {
            std::cerr << "Error: Failed to export tasks.\n";
        }
    } else if (choice == "2") {
        std::cout << "Importing tasks...\n";
        // Assuming FileManager has an import method
        if (fileManager_.loadTodoList()) {
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

void ConsoleUI::handleInput(std::string input) {
    if (commandMap_.count(input)) {
        commandMap_[input]();
    } else {
        std::cout << "Invalid command. Please enter a number between 1 and 5.\n";
    }
}

// Getters for private members (needed for consoleUI access)
TaskManager& ConsoleUI::getTaskManager() {
    return taskManager_;
}

FileManager& ConsoleUI::getFileManager() {
    return fileManager_;
}

// Helper function to parse due date string to time_point
std::chrono::system_clock::time_point ConsoleUI::parseDueDate(const std::string& dueDateStr) {
    std::tm timeVal = {};
    
    // Parse YYYY-MM-DD HH:MM:SS format
    int parsed = std::sscanf(dueDateStr.c_str(), "%d-%d-%d %d:%d:%d",
                             &timeVal.tm_year, &timeVal.tm_mon, &timeVal.tm_mday,
                             &timeVal.tm_hour, &timeVal.tm_min, &timeVal.tm_sec);
    
    if (parsed != 6) {
        std::cerr << "Error: Invalid due date format. Expected YYYY-MM-DD HH:MM:SS, got: " << dueDateStr << std::endl;
        // Return current time as fallback
        return std::chrono::system_clock::now();
    }
    
    // Validate date components
    if (timeVal.tm_year < 1900 || timeVal.tm_mon < 1 || timeVal.tm_mon > 12 ||
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
}
