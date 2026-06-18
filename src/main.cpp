
#include <iostream>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

#include "fileManager.hpp"
#include "taskManager.hpp"
#include "consoleUI.hpp"
#include "cliHandler.hpp"
#include "settings.hpp"
#include "logger.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // Define the path for the local todo list file
    const std::string todoFilePath = ".todo_list.txt";
    const std::string settingsFilePath = ".settings.txt";

    // This error should be suppressed in the future 
    if (!loadSettings(settingsFilePath)) {
        Logger::log(Logger::LogLevel::INFO, "No settings found: using default settings.");
    }

    FileManager fileManager(todoFilePath);

    TaskManager taskManager(fileManager);

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("menu,m", "Run in interactive menu mode")
        ("add,a", "Add a new task")
        ("list,l", "List all tasks")
        ("complete,c", "Complete a task")
        ("export,e", "Export tasks to file")
        ("import,i", "Import tasks from file")
        ("exit,x", "Exit the application")
        ("priority,p", po::value<int>()->default_value(5), "Priority level (0-10) for add task")
        ("task-id,T", po::value<int>(), "Task ID for complete task operation")
        ("name,n", po::value<std::string>(), "Task name for add task operation")
        ("description,d", po::value<std::string>(), "Task description for add task operation")
        ("due-date,u", po::value<std::string>(), "Due date in YYYY-MM-DD HH:MM:SS format for add task")
        // Filter options for list command
        ("due-date-min,dm", po::value<std::string>(), "Minimum due date (YYYY-MM-DD HH:MM:SS) for filtering")
        ("due-date-max,dM", po::value<std::string>(), "Maximum due date (YYYY-MM-DD HH:MM:SS) for filtering")
        ("priority-min,pm", po::value<int>(), "Minimum priority (0-10) for filtering")
        ("priority-max,pM", po::value<int>(), "Maximum priority (0-10) for filtering");

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
        po::notify(vm);
    } catch (const po::error& ex) {
        Logger::log(Logger::LogLevel::ERROR, ex.what());
        std::cout << "\nUsage: " << desc << std::endl;
        
        exit(1);
    }

    std::cout << "Todo List Application Started." << std::endl;

    // Check if menu mode is enabled via CLI
    if (vm.count("menu") > 0) {
        // Run in interactive menu mode using original ConsoleUI
        ConsoleUI consoleUI(taskManager, fileManager);
        while (true) {
            consoleUI.displayMenu();
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);
            consoleUI.handleInput(input);
        }
    } else {
        // Run with CLI options using new CLIHandler
        CLIHandler cliHandler(taskManager, fileManager, vm);
        cliHandler.execute();
    }

    return 0;
}