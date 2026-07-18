#include "parse_arguments/parser.hpp"

bool Parser::parse(int argc, char* argv[], std::shared_ptr<Settings> settings){
    try {
        po::store(po::command_line_parser(argc, argv).options(getOptions()).allow_unregistered().run(), vm);        
        po::notify(vm);
        // Set verbose flag
        if (vm.count("verbose")) {
            settings.get()->setVerbose(verboseFlag_);
        }
        
        // Set max tasks per file
        if (vm.count("max-tasks")) {
            settings.get()->setMaxTasksPerFile(maxTasks_);
        }
        
    } catch (const po::error& ex) {
        std::cerr << "Error parsing command-line arguments: " << ex.what() << std::endl;
        return false;
    }
    
    return true;
}