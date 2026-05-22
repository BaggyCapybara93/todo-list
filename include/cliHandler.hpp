#pragma once
#include "taskManager.hpp"
#include "fileManager.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

class CLIHandler {
private:
    TaskManager& taskManager_;
    FileManager& fileManager_;
    po::variables_map config_;

    // CLI command handlers
    void handleAddTask();
    void handleListTasks();
    void handleCompleteTask();
    void handleFileOperations();
    void handleHelp();
    void handleExit();

public:
    CLIHandler(TaskManager& tm, FileManager& fm, po::variables_map config_);
    
    // Execute CLI commands
    void execute();
    
    // Display help
    void displayHelp(const po::variables_map& vm);
};
