#include "cli.hpp"
#include "ui.hpp"

CLI::CLI(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm, po::variables_map config)
    : taskManager_(std::move(tm)), fileManager_(std::move(fm)), config_(config)
    {parseVM(config_);}

void CLI::execute() {
    try {

        if (helpRequested_) {
            displayHelp();
            return;
        }

        switch(command_) {
            case ADD: handleAddTask(); break;
            case LIST: handleListTasks(); break;
            case COMPLETE: handleCompleteTask(); break;
            case ADDTAG: handleAddTag(); break;
            case REMOVETAG: handleRemoveTag(); break;
            case REPEAT: handleSetRepeatTask(); break;
            case IMPORT:
            case EXPORT: handleFileOperations(); break;

            case HELP:
                displayHelp();
                break;

            default:
                Logger::log(Logger::LogLevel::ERROR, "No command specified.");
                Logger::log(Logger::LogLevel::INFO, "Use --help for more information.");
                handleExit();
                break;
        }

    } catch(const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR,
            "Unknown error occurred while handling command: " + std::string(e.what()));
    }
}

//Move this later into a parse_arguments class
void CLI::parseVM(const po::variables_map& vm) {

    bool wantsHelp = vm.count("help") > 0;

    if (vm.count("add") > 0) {
        command_ = ADD;
    } else if (vm.count("list") > 0) {
        command_ = LIST;
    } else if (vm.count("complete") > 0) {
        command_ = COMPLETE;
    } else if (vm.count("add-tag") > 0 || vm.count("tag-name") > 0) {
        command_ = ADDTAG;
    } else if (vm.count("remove-tag") > 0 || vm.count("tag-name") > 0 || vm.count("tag-id") > 0) {
        command_ = REMOVETAG;
    } else if (vm.count("export") > 0) {
        command_ = EXPORT;
    } else if (vm.count("import") > 0) {
        command_ = IMPORT;
    } else if (wantsHelp) {
        command_ = HELP;
    } else {
        command_ = UNKNOWN;
    }

    helpRequested_ = wantsHelp; //This isnt great but is fine for now
}

void CLI::displayHelp() {
    UI::instance().println("\nTodo List Application - Help", Color::Cyan);
    UI::instance().println("============================================\n", Color::White);

    switch(command_){
        case HELP: {
            UI::instance().println("General Help:", Color::Green);
            UI::instance().println("  This application manages todo lists with the following commands:\n");
            UI::instance().println("  --help, -h          Show this help message");
            UI::instance().println("  --menu, -m          Run in interactive menu mode");
            UI::instance().println("  --verbose, -v       Print verbose output");
            UI::instance().println("  --add, -a           Add a new task");
            UI::instance().println("  --list, -l          List all tasks");
            UI::instance().println("  --complete, -c      Complete a task");
            UI::instance().println("  --add-tag, -tg      Add a new tag");
            UI::instance().println("  --remove-tag, -rt   Remove a tag");
            UI::instance().println("  --export, -e        Export tasks to file");
            UI::instance().println("  --import, -i        Import tasks from file");
            UI::instance().println("  --exit, -x          Exit the application\n");
            UI::instance().println("============================================", Color::White);
            UI::instance().println("EXAMPLES", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("  Add a task:");
            UI::instance().println("    --add --name \"Buy groceries\" --description \"Milk, eggs, bread\" --due-date 2026-06-25 12:00:00 --priority 7\n");
            UI::instance().println("  List pending tasks:");
            UI::instance().println("    --list\n");
            UI::instance().println("  List tasks with filters:");
            UI::instance().println("    --list --due-date-min 2026-06-20 00:00:00 --priority-min 5\n");
            UI::instance().println("  Complete a task:");
            UI::instance().println("    --complete --task-id 1\n");
            UI::instance().println("  Add a tag:");
            UI::instance().println("    --add-tag --tag-name \"Work\" --tag-description \"Work-related tasks\"\n");
            UI::instance().println("  Remove a tag:");
            UI::instance().println("    --remove-tag --tag-name \"Work\"\n");
            UI::instance().println("  Export all tasks:");
            UI::instance().println("    --export\n");
            break;
        }

        case ADD: {
            UI::instance().println("ADD TASK (--add, -a)", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("Add a new task to the todo list.\n");
            UI::instance().println("  Required options:");
            UI::instance().println("    --name, -n          Task name (required)");
            UI::instance().println("    --description, -d   Task description (required)");
            UI::instance().println("    --due-date, -u      Due date in YYYY-MM-DD HH:MM:SS format (required)\n");
            UI::instance().println("  Optional options:");
            UI::instance().println("    --priority, -p      Priority level (0-10), default: 5\n");
            UI::instance().println("  Example:");
            UI::instance().println("    --add --name \"Complete project\" --description \"Finish all features\" --due-date 2026-07-01 18:00:00 --priority 9");
            break;
        }

        case LIST: {
            UI::instance().println("LIST TASKS (--list, -l)", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("List all pending tasks in the todo list.\n");
            UI::instance().println("  Options:");
            UI::instance().println("    Without filters: Shows all pending tasks\n");
            UI::instance().println("    With filters:");
            UI::instance().println("      --due-date-min, -dm    Minimum due date (YYYY-MM-DD HH:MM:SS)");
            UI::instance().println("      --due-date-max, -dM    Maximum due date (YYYY-MM-DD HH:MM:SS)");
            UI::instance().println("      --priority-min, -pm    Minimum priority (0-10)");
            UI::instance().println("      --priority-max, -pM    Maximum priority (0-10)\n");
            UI::instance().println("  Examples:");
            UI::instance().println("    --list");
            UI::instance().println("    --list --due-date-min 2026-06-20 00:00:00");
            UI::instance().println("    --list --priority-min 5");
            UI::instance().println("    --list --priority-min 3 --priority-max 7");
            break;
        }

        case COMPLETE: {
            UI::instance().println("COMPLETE TASK (--complete, -c)", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("Mark a task as complete.\n");
            UI::instance().println("  Required options:");
            UI::instance().println("    --task-id, -T       Task ID to complete (required)\n");
            UI::instance().println("  Example:");
            UI::instance().println("    --complete --task-id 1");
            break;
        }

        case ADDTAG: {
            UI::instance().println("ADD TAG (--add-tag, -at)", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("Add a new tag to organize tasks.\n");
            UI::instance().println("  Required options:");
            UI::instance().println("    --tag-name, -tn     Tag name (required)");
            UI::instance().println("    --tag-description, -td  Tag description (required)\n");
            UI::instance().println("  Example:");
            UI::instance().println("    --add-tag --tag-name \"Work\" --tag-description \"Work-related tasks\"");
            break;
        }

        case REMOVETAG: {
            UI::instance().println("REMOVE TAG (--remove-tag, -rt)", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("Remove a tag from the todo list.\n");
            UI::instance().println("  Required options:");
            UI::instance().println("    --tag-name, -tn     Tag name (required)");
            UI::instance().println("    --tag-id, -tid      Tag ID (alternative to name)\n");
            UI::instance().println("  Example:");
            UI::instance().println("    --remove-tag --tag-name \"Work\"");
            break;
        }

        case EXPORT: {
            UI::instance().println("EXPORT TASKS (--export, -e)", Color::Green);
            UI::instance().println("============================================\n", Color::White);

            UI::instance().println("Export all tasks to the todo list file.\n");

            UI::instance().println("  This saves all current tasks to:");
            UI::instance().println("    " + fileManager_.get()->getTodoFilePath() + "\n");

            UI::instance().println("Use this to backup your tasks or transfer them to another device.");
            break;
        }

        case IMPORT: {
            UI::instance().println("IMPORT TASKS (--import, -i)", Color::Green);
            UI::instance().println("============================================\n", Color::White);

            UI::instance().println("Import tasks from the todo list file.\n");

            UI::instance().println("  This loads tasks from:");
            UI::instance().println("    " + fileManager_.get()->getTodoFilePath() + "\n");

            UI::instance().println("Use this to restore your tasks or import from another device.");
            break;
        }

        default:
        case UNKNOWN: {
            UI::instance().println("COMMANDS", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("  --help, -h          Show this help message");
            UI::instance().println("  --menu, -m          Run in interactive menu mode");
            UI::instance().println("  --add, -a           Add a new task");
            UI::instance().println("  --list, -l          List all tasks");
            UI::instance().println("  --complete, -c      Complete a task");
            UI::instance().println("  --repeat, -r        Set repeat interval for a task");
            UI::instance().println("  --add-tag, -tg      Add a new tag");
            UI::instance().println("  --remove-tag, -rt   Remove a tag");
            UI::instance().println("  --export, -e        Export tasks to file");
            UI::instance().println("  --import, -i        Import tasks from file");
            UI::instance().println("  --exit, -x          Exit the application\n");
            UI::instance().println("============================================", Color::White);
            UI::instance().println("QUICK START", Color::Green);
            UI::instance().println("============================================\n", Color::White);
            UI::instance().println("  1. Add your first task:");
            UI::instance().println("     --add --name \"Hello World\" --description \"My first task\" --due-date 2026-06-25 12:00:00\n");
            UI::instance().println("  2. List all tasks:");
            UI::instance().println("     --list\n");
            UI::instance().println("  3. Complete a task:");
            UI::instance().println("     --complete --task-id 1\n");
            UI::instance().println("  4. Set repeat interval for a task:");
            UI::instance().println("     --repeat --task-id 1 --repeat-interval DAILY\n");
            UI::instance().println("  5. Export your tasks:");
            UI::instance().println("     --export");
            break;
        }
    }
}