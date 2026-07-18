
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
    // Define the path for the local todo list file
    const std::string todoFilePath = ".todo_list.txt";
    const std::string settingsFilePath = ".settings.txt";

    // Load settings from file (returns default settings if file doesn't exist)
    auto settings = std::make_shared<Settings>();
    settings.get()->loadSettings(settingsFilePath);
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