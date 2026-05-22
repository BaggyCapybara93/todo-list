
#include <iostream>
#include <string>
#include <chrono>
#include "fileManager.hpp"
#include "taskManager.hpp"
#include "consoleUI.hpp"
#include "cliHandler.hpp"
#include "settings.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // Define the path for the local todo list file
    const std::string todoFilePath = ".todo_list.txt";
    const std::string settingsFilePath = ".settings.txt";

    // 1. Initialize Settings
    if (!loadSettings(settingsFilePath)) {
        std::cout << "Using default settings." << std::endl;
    }

    // 2. Initialize FileManager
    FileManager fileManager(todoFilePath);

    // 3. Initialize TaskManager, passing the FileManager reference
    TaskManager taskManager(fileManager);

    // 4. Initialize CLI options using Boost
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
        ("due-date,u", po::value<std::string>(), "Due date in YYYY-MM-DD HH:MM:SS format");

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
        po::notify(vm);
    } catch (const po::error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
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