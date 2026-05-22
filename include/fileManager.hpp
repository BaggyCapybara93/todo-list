#pragma once

#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "task.hpp"

class FileManager {
public:
    FileManager(const std::string& todoFilePath);

    //Getters
    const std::string& getTodoFilePath() const { return todoFilePath_; }

    std::optional<std::vector<std::shared_ptr<Task>>> loadTodoList();
    
    bool saveTodoList(const std::vector<std::shared_ptr<Task>>& tasks);

    // Helper methods
    bool isValidFile(const std::string& path) const;
    
    std::string escapeJsonString(const std::string& str) const;

private:
    std::string todoFilePath_;
};