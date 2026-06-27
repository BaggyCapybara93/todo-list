
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

#include "file_manager/fileManager.hpp"
#include "task/manager.hpp"
#include "console/console.hpp"
#include "cli/cli.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "ui.hpp"
#include "tag/manager.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // Define the path for the local todo list file
    const std::string todoFilePath = ".todo_list.txt";
    const std::string settingsFilePath = ".settings.txt";

    // Load settings from file (returns default settings if file doesn't exist)
    auto settings = std::make_shared<Settings>(loadSettings(settingsFilePath));
    auto fileManager = std::make_shared<FileManager>(todoFilePath);
    auto tagManager = std::make_shared<TagManager>(settings);
    auto taskManager = std::make_shared<TaskManager>(fileManager, tagManager);

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("menu,m", "Run in interactive menu mode")
        ("verbose,v", po::bool_switch(&setting_.verbose)->default_value(false),"Print verbose output")
        ("add,a", "Add a new task")
        ("list,l", "List all tasks")
        ("complete,c", "Complete a task")
        ("export,e", "Export tasks to file")
        ("import,i", "Import tasks from file")
        ("priority,p", po::value<int>()->default_value(5), "Priority level (0-10) for add task")
        ("task-id,T", po::value<int>(), "Task ID for complete task operation")
        ("name,n", po::value<std::string>(), "Task name for add task operation")
        ("description,d", po::value<std::string>(), "Task description for add task operation")
        ("due-date,u", po::value<std::string>(), "Due date in YYYY-MM-DD HH:MM:SS format for add task")
        // Tag options
        ("add-tag,at", "Add a new tag")
        ("remove-tag,rt", "Remove a tag")
        ("tag-name,tn", po::value<std::string>(), "Tag name for add/remove tag operation")
        ("tag-description,td", po::value<std::string>(), "Tag description for add tag operation")
        ("tag-id,tid", po::value<int>(), "Tag ID for remove tag operation")
        // Repeat task options
        ("repeat,r", "Set repeat interval for a task")
        ("repeat-interval", po::value<std::string>(), "Repeat interval (NEVER, DAILY, WEEKLY, MONTHLY, YEARLY) for repeat task")
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
        UI::instance().println("Error parsing command line options.");
        
        exit(1);
    }

    // Check if menu mode is enabled via CLI
    if (vm.count("menu") > 0) {
        // Run in interactive menu mode using original ConsoleUI
        Console console(taskManager, fileManager);
        while (true) {
            console.displayMenu();
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);
            console.handleInput(input);
        }
    } else {
        // Run with CLI options using new CLIHandler
        CLI cli(taskManager, fileManager, vm);
        cli.execute();
    }

    return 0;
}