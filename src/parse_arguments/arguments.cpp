#include <iostream>
#include <vector>
#include <boost/program_options.hpp>

#include "parse_arguments/parser.hpp"

namespace po = boost::program_options;
po::options_description Parser::getOptions() {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help,h", "Show help message")
        ("menu,m", "Run in interactive menu mode")
        ("verbose,v", po::bool_switch(&verboseFlag_)->default_value(false),"Print verbose output")
        ("max-tasks,mt", po::value(&maxTasks_)->default_value(1000),"Maximum number of tasks to display")
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
        ("subtask", po::value<int>(), "Task ID of the primary task to attach the new task as a subtask to")
        ("dependency", po::value<int>(), "Task ID of the primary task to attach the new task as a dependency to")
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

    return desc;
}