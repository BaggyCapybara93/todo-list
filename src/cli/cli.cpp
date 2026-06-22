#include "cli.hpp"


CLI::CLI(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm, po::variables_map config)
    : taskManager_(std::move(tm)), fileManager_(std::move(fm)), config_(config) {
}

void CLI::execute() {
    try{
        if (config_.count("help") > 0) {
            handleHelp();
        } else if (config_.count("add") > 0) {
            handleAddTask();
        } else if (config_.count("list") > 0) {
            handleListTasks();
        } else if (config_.count("complete") > 0) {
            handleCompleteTask();
        } else if (config_.count("add-tag") > 0 || config_.count("tag-name") > 0) {
            handleAddTag();
        } else if (config_.count("remove-tag") > 0 || config_.count("tag-name") > 0 || config_.count("tag-id") > 0) {
            handleRemoveTag();
        }  else if (config_.count("export") > 0 || config_.count("import") > 0) {
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

void CLI::displayHelp(const po::variables_map& vm) {
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
        std::cout << "  --add-tag, -tg      Add a new tag\n";
        std::cout << "  --remove-tag, -rt   Remove a tag\n";
        std::cout << "  --export, -e        Export tasks to file\n";
        std::cout << "  --import, -i        Import tasks from file\n";
        std::cout << "  --exit, -x          Exit the application\n\n";
        
        std::cout << "========================================\n";
        std::cout << "EXAMPLES\n";
        std::cout << "========================================\n\n";
        std::cout << "  Add a task:\n";
        std::cout << "    --add --name \"Buy groceries\" --description \"Milk, eggs, bread\" --due-date 2026-06-25 12:00:00 --priority 7\n\n";
        std::cout << "  List pending tasks:\n";
        std::cout << "    --list\n\n";
        std::cout << "  List tasks with filters:\n";
        std::cout << "    --list --due-date-min 2026-06-20 00:00:00 --priority-min 5\n\n";
        std::cout << "  Complete a task:\n";
        std::cout << "    --complete --task-id 1\n\n";
        std::cout << "  Add a tag:\n";
        std::cout << "    --add-tag --tag-name \"Work\" --tag-description \"Work-related tasks\"\n\n";
        std::cout << "  Remove a tag:\n";
        std::cout << "    --remove-tag --tag-name \"Work\"\n\n";
        std::cout << "  Export all tasks:\n";
        std::cout << "    --export\n\n";
    }
    else if (vm.count("add") > 0) {
        std::cout << "========================================\n";
        std::cout << "ADD TASK (--add, -a)\n";
        std::cout << "========================================\n\n";
        std::cout << "Add a new task to the todo list.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --name, -n          Task name (required)\n";
        std::cout << "    --description, -d   Task description (required)\n";
        std::cout << "    --due-date, -u      Due date in YYYY-MM-DD HH:MM:SS format (required)\n\n";
        std::cout << "  Optional options:\n";
        std::cout << "    --priority, -p      Priority level (0-10), default: 5\n\n";
        std::cout << "  Example:\n";
        std::cout << "    --add --name \"Complete project\" --description \"Finish all features\" --due-date 2026-07-01 18:00:00 --priority 9\n";
    }
    else if (vm.count("list") > 0) {
        std::cout << "========================================\n";
        std::cout << "LIST TASKS (--list, -l)\n";
        std::cout << "========================================\n\n";
        std::cout << "List all pending tasks in the todo list.\n\n";
        std::cout << "  Options:\n";
        std::cout << "    Without filters: Shows all pending tasks\n\n";
        std::cout << "    With filters: Use the following options to filter pending tasks:\n";
        std::cout << "      --due-date-min, -dm    Minimum due date (YYYY-MM-DD HH:MM:SS)\n";
        std::cout << "      --due-date-max, -dM    Maximum due date (YYYY-MM-DD HH:MM:SS)\n";
        std::cout << "      --priority-min, -pm    Minimum priority (0-10)\n";
        std::cout << "      --priority-max, -pM    Maximum priority (0-10)\n\n";
        std::cout << "  Examples:\n";
        std::cout << "    List all pending tasks:\n";
        std::cout << "      --list\n\n";
        std::cout << "    List tasks due after a specific date:\n";
        std::cout << "      --list --due-date-min 2026-06-20 00:00:00\n\n";
        std::cout << "    List tasks with priority >= 5:\n";
        std::cout << "      --list --priority-min 5\n\n";
        std::cout << "    List tasks with priority between 3 and 7:\n";
        std::cout << "      --list --priority-min 3 --priority-max 7\n";
    }
    else if (vm.count("complete") > 0) {
        std::cout << "========================================\n";
        std::cout << "COMPLETE TASK (--complete, -c)\n";
        std::cout << "========================================\n\n";
        std::cout << "Mark a task as complete.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --task-id, -T       Task ID to complete (required)\n\n";
        std::cout << "  Example:\n";
        std::cout << "    --complete --task-id 1\n";
    }
    else if (vm.count("add-tag") > 0) {
        std::cout << "========================================\n";
        std::cout << "ADD TAG (--add-tag, -at)\n";
        std::cout << "========================================\n\n";
        std::cout << "Add a new tag to organize tasks.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --tag-name, -tn     Tag name (required)\n";
        std::cout << "    --tag-description, -td  Tag description (required)\n\n";
        std::cout << "  Example:\n";
        std::cout << "    --add-tag --tag-name \"Work\" --tag-description \"Work-related tasks\"\n";
    }
    else if (vm.count("remove-tag") > 0) {
        std::cout << "========================================\n";
        std::cout << "REMOVE TAG (--remove-tag, -rt)\n";
        std::cout << "========================================\n\n";
        std::cout << "Remove a tag from the todo list.\n\n";
        std::cout << "  Required options:\n";
        std::cout << "    --tag-name, -tn     Tag name (required)\n";
        std::cout << "    --tag-id, -tid     Tag ID (alternative to name)\n\n";
        std::cout << "  Example:\n";
        std::cout << "    --remove-tag --tag-name \"Work\"\n";
    }
    else if (vm.count("export") > 0) {
        std::cout << "========================================\n";
        std::cout << "EXPORT TASKS (--export, -e)\n";
        std::cout << "========================================\n\n";
        std::cout << "Export all tasks to the todo list file.\n\n";
        std::cout << "  This saves all current tasks to: " << fileManager_.get()->getTodoFilePath() << "\n\n";
        std::cout << "  Use this to backup your tasks or transfer them to another device.\n";
    }
    else if (vm.count("import") > 0) {
        std::cout << "========================================\n";
        std::cout << "IMPORT TASKS (--import, -i)\n";
        std::cout << "========================================\n\n";
        std::cout << "Import tasks from the todo list file.\n\n";
        std::cout << "  This loads tasks from: " << fileManager_.get()->getTodoFilePath() << "\n\n";
        std::cout << "  Use this to restore your tasks or import from another device.\n";
    }
    else {
        std::cout << "========================================\n";
        std::cout << "COMMANDS\n";
        std::cout << "========================================\n\n";
        std::cout << "  --help, -h          Show this help message\n";
        std::cout << "  --menu, -m          Run in interactive menu mode\n";
        std::cout << "  --add, -a           Add a new task\n";
        std::cout << "  --list, -l          List all tasks\n";
        std::cout << "  --complete, -c      Complete a task\n";
        std::cout << "  --add-tag, -tg      Add a new tag\n";
        std::cout << "  --remove-tag, -rt   Remove a tag\n";
        std::cout << "  --export, -e        Export tasks to file\n";
        std::cout << "  --import, -i        Import tasks from file\n";
        std::cout << "  --exit, -x          Exit the application\n\n";
        
        std::cout << "========================================\n";
        std::cout << "QUICK START\n";
        std::cout << "========================================\n\n";
        std::cout << "  1. Add your first task:\n";
        std::cout << "     --add --name \"Hello World\" --description \"My first task\" --due-date 2026-06-25 12:00:00\n\n";
        std::cout << "  2. List all tasks:\n";
        std::cout << "     --list\n\n";
        std::cout << "  3. Complete a task:\n";
        std::cout << "     --complete --task-id 1\n\n";
        std::cout << "  4. Export your tasks:\n";
        std::cout << "     --export\n";
    }
}
