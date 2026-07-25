
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

#include "file_manager/fileManager.hpp"
#include "task/manager.hpp"
#include "cli/cli.hpp"
#include "settings.hpp"
#include "logger.hpp"
#include "ui.hpp"
#include "tag/manager.hpp"
#include "parse_arguments/parser.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // Define default paths for the local todo list and settings files
    const std::string defaultTodoFilePath = ".todo_list.txt";
    const std::string defaultSettingsFilePath = ".settings.txt";

    // Load settings from file (returns default settings if file doesn't exist)
    auto settings = std::make_shared<Settings>();
    settings.get()->loadSettings(defaultSettingsFilePath);
    
    // Get file paths from settings or use defaults
    std::string todoFilePath = settings.get()->getTodoFilePath();
    if (todoFilePath.empty()) {
        todoFilePath = defaultTodoFilePath;
    }
    
    std::string settingsFilePath = settings.get()->getSettingsFilePath();
    if (settingsFilePath.empty()) {
        settingsFilePath = defaultSettingsFilePath;
    }
    
    auto fileManager = std::make_shared<FileManager>(todoFilePath);
    auto tagManager = std::make_shared<TagManager>(settings);
    auto taskManager = std::make_shared<TaskManager>(fileManager, tagManager, settings);

    Parser parser;
    if (!parser.parse(argc, argv, settings)) {
        std::cerr << "Error parsing command-line arguments." << std::endl;
    }

    // Run with CLI options using new CLIHandler
    CLI cli(taskManager, fileManager, parser.getVariablesMap());
    cli.execute();

    return 0;
}