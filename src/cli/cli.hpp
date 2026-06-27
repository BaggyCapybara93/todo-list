#pragma once
#include "task/manager.hpp"
#include "file_manager/fileManager.hpp"
#include "utils/utils.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <memory>

namespace po = boost::program_options;

enum CLICommand{
    HELP,
    ADD,
    LIST,
    COMPLETE,
    ADDTAG,
    REMOVETAG,
    EXPORT,
    IMPORT,
    UNKNOWN
};

class CLI {
    private:
        std::shared_ptr<TaskManager> taskManager_;
        std::shared_ptr<FileManager> fileManager_;
        po::variables_map config_;
        CLICommand command_;

        // CLI command handlers
        void handleAddTask();
        void handleListTasks();
        void handleCompleteTask();
        void handleSetRepeatTask();
        void handleFileOperations();
        void handleAddTag();
        void handleRemoveTag();
        void handleHelp();
        void handleExit();

        /**
         * @brief Parse variables map for commands
         */
        void parseVM(const po::variables_map& vm);

    public:
        CLI(std::shared_ptr<TaskManager> tm, std::shared_ptr<FileManager> fm, po::variables_map config_);
    
        // Execute CLI commands
        void execute();
        
        // Display help
        void displayHelp();
};