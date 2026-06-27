#pragma once

#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "task/task.hpp"

class FileManager {
public:
    FileManager(const std::string& todoFilePath);

    //Getters
    const std::string& getTodoFilePath() const { return todoFilePath_; }

    /**
         * @brief Loads todo list from directory
         * @return Returns null if no todolist is found or std::vector of tasks
    */
    std::optional<std::vector<std::shared_ptr<Task>>> loadTodoList();
    
    /**
         * @brief Save todolist to file
         * @param tasks Vector of tasks to save
         * @return True for successful saving of tasks or false in event of failure
    */
    bool saveTodoList(const std::vector<std::shared_ptr<Task>>& tasks);
    
    /**
         * @brief Handles escape json string
         * @param str String to be used
         * @return String created
    */
    std::string escapeJsonString(const std::string& str) const;

private:
    std::string todoFilePath_;
};