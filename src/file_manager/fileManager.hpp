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
    
    /**
         * @brief Checks if file path is valid and accessible
         * @param filePath Path to check
         * @return True if valid, false otherwise
    */
    bool isValidFilePath(const std::string& filePath) const;
    
    /**
         * @brief Checks file permissions for reading
         * @param filePath Path to check
         * @return True if readable, false otherwise
    */
    bool isFileReadable(const std::string& filePath) const;
    
    /**
         * @brief Checks file permissions for writing
         * @param filePath Path to check
         * @return True if writable, false otherwise
    */
    bool isFileWritable(const std::string& filePath) const;
};