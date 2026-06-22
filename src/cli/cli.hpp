#pragma once
#include "taskManager.hpp"
#include "fileManager.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <memory>

namespace po = boost::program_options;

class CLI {
    private:
        std::shared_ptr<TaskManager> taskManager_;
        std::shared_ptr<FileManager> fileManager_;
        po::variables_map config_;

        // CLI command handlers
        void handleAddTask();
        void handleListTasks();
        void handleCompleteTask();
        void handleFileOperations();
        void handleAddTag();
        void handleRemoveTag();
        void handleHelp();
        void handleExit();

    public:
        CLI(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm, po::variables_map config_);
    
        // Execute CLI commands
        void execute();
        
        // Display help
        void displayHelp(const po::variables_map& vm);
};