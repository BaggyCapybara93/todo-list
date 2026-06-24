#include "cli.hpp"
#include "ui.hpp"

void CLI::handleAddTask() {
    try {
        std::string name, description, dueDateStr;
        int priority;

        if (config_.count("name") > 0 &&
            config_.count("description") > 0 &&
            config_.count("due-date") > 0) {

            name = config_["name"].as<std::string>();
            description = config_["description"].as<std::string>();
            dueDateStr = config_["due-date"].as<std::string>();
            priority = config_.count("priority") > 0 ? config_["priority"].as<int>() : 5;

        } else {
            UI::instance().println("[WARNING] Missing required arguments for add task.", Color::Yellow);
            UI::instance().println("Use --name, --description, and --due-date flags.", Color::Cyan);
            return;
        }

        if (priority < 0 || priority > 10) {
            UI::instance().println("[WARNING] Priority must be between 0 and 10.", Color::Yellow);
            return;
        }

        auto dueDate = parseDueDate(dueDateStr);

        taskManager_->addTask(name, description, priority, dueDate);

        UI::instance().println("Task added successfully.", Color::Green);

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error adding task: " + std::string(e.what()), Color::Red);
    }
}

void CLI::handleListTasks() {
    try {
        std::shared_ptr<std::vector<std::shared_ptr<Task>>> tasksToDisplay;

        bool hasDueDateMin = config_.count("due-date-min") > 0;
        bool hasDueDateMax = config_.count("due-date-max") > 0;
        bool hasPriorityMin = config_.count("priority-min") > 0;
        bool hasPriorityMax = config_.count("priority-max") > 0;

        if (hasDueDateMin || hasDueDateMax || hasPriorityMin || hasPriorityMax) {

            UI::instance().println("\n--- Filtered Pending Tasks ---", Color::Cyan);

            auto dueDateMin = hasDueDateMin ? parseDueDate(config_["due-date-min"].as<std::string>())
                                            : std::chrono::system_clock::time_point();

            auto dueDateMax = hasDueDateMax ? parseDueDate(config_["due-date-max"].as<std::string>())
                                            : std::chrono::system_clock::time_point::max();

            int priorityMin = hasPriorityMin ? config_["priority-min"].as<int>() : 0;
            int priorityMax = hasPriorityMax ? config_["priority-max"].as<int>() : 10;

            if (priorityMin < 0 || priorityMin > 10 ||
                priorityMax < 0 || priorityMax > 10 ||
                priorityMin > priorityMax) {

                UI::instance().println("[WARNING] Priority must be between 0 and 10, and min <= max.", Color::Yellow);
                return;
            }

            tasksToDisplay = taskManager_->filterTasksByDueDateAndPriority(
                dueDateMin, dueDateMax, priorityMin, priorityMax
            );

        } else {
            UI::instance().println("\n--- Pending Tasks ---", Color::Cyan);
            tasksToDisplay = taskManager_->getPendingTasks();
        }

        if (tasksToDisplay->empty()) {
            UI::instance().println("No pending tasks found.", Color::Yellow);
        } else {
            for (const auto& task : *tasksToDisplay) {
                UI::instance().println(task->toString(), Color::White);
            }
        }

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error listing tasks: " + std::string(e.what()), Color::Red);
    }
}

void CLI::handleCompleteTask() {
    try {
        if (!config_.count("task-id")) {
            UI::instance().println("[WARNING] Missing required argument for complete task.", Color::Yellow);
            UI::instance().println("Use --task-id flag.", Color::Cyan);
            return;
        }

        int id = config_["task-id"].as<int>();

        if (taskManager_->completeTask(id)) {
            UI::instance().println("Task ID " + std::to_string(id) + " marked as complete.", Color::Green);
        } else {
            UI::instance().println("[ERROR] Task ID " + std::to_string(id) + " not found.", Color::Red);
        }

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error completing task: " + std::string(e.what()), Color::Red);
    }
}

void CLI::handleFileOperations() {
    try {
        if (config_.count("export") > 0) {

            UI::instance().println("Exporting tasks...", Color::Cyan);

            auto tasks = taskManager_->getTasks();

            if (tasks && !tasks->empty()) {
                if (fileManager_->saveTodoList(*tasks)) {
                    UI::instance().println("Tasks exported successfully to " + fileManager_->getTodoFilePath(), Color::Green);
                } else {
                    UI::instance().println("[ERROR] Failed to export tasks.", Color::Red);
                }
            }

        } else if (config_.count("import") > 0) {

            UI::instance().println("Importing tasks...", Color::Cyan);

            if (fileManager_->loadTodoList()) {
                UI::instance().println("Tasks imported successfully.", Color::Green);
            } else {
                UI::instance().println("[ERROR] Failed to import tasks.", Color::Red);
            }

        } else {
            UI::instance().println("[WARNING] Missing required argument for file operations.", Color::Yellow);
            UI::instance().println("Use --export or --import flag.", Color::Cyan);
        }

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error handling file operations: " + std::string(e.what()), Color::Red);
    }
}

void CLI::handleHelp() {
    CLI::displayHelp(config_);
}

void CLI::handleExit() {
    UI::instance().println("Exiting...", Color::Cyan);
    exit(0);
}

void CLI::handleAddTag() {
    try {
        if (config_.count("tag-name") > 0 &&
            config_.count("tag-description") > 0) {

            std::string name = config_["tag-name"].as<std::string>();
            std::string description = config_["tag-description"].as<std::string>();

            if (taskManager_->addTag(name, description)) {
                UI::instance().println("Tag '" + name + "' added successfully.", Color::Green);
            } else {
                UI::instance().println("[ERROR] Failed to add tag.", Color::Red);
            }

        } else {
            UI::instance().println("[WARNING] Missing required arguments for add tag.", Color::Yellow);
            UI::instance().println("Use --tag-name and --tag-description flags.", Color::Cyan);
        }

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error adding tag: " + std::string(e.what()), Color::Red);
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
            UI::instance().println("[WARNING] Missing required arguments for remove tag.", Color::Yellow);
            UI::instance().println("Use --tag-name or --tag-id flag.", Color::Cyan);
            return;
        }

        if (taskManager_->removeTag(tagName, tagId)) {
            UI::instance().println("Tag removed successfully.", Color::Green);
        } else {
            UI::instance().println("[ERROR] Failed to remove tag.", Color::Red);
        }

    } catch (const std::exception& e) {
        UI::instance().println("[ERROR] Unknown error removing tag: " + std::string(e.what()), Color::Red);
    }
}
