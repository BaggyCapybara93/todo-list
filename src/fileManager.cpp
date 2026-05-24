#include "fileManager.hpp"
#include "utils.hpp"
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

FileManager::FileManager(const std::string& todoFilePath) : todoFilePath_(todoFilePath) {}

std::string FileManager::escapeJsonString(const std::string& str) const {
    return nlohmann::detail::escape(str);
}

std::optional<std::vector<std::shared_ptr<Task>>> FileManager::loadTodoList() {
    try{
        std::vector<std::shared_ptr<Task>> tasks;
        std::ifstream file(todoFilePath_);
        
        if(!isValidFilePath(todoFilePath_)) {
            return std::nullopt;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            
            try {
                json j = json::parse(line);
                auto task = std::make_shared<Task>();
                task.get()->setId(j["id"].get<int>());
                task.get()->setName(j["name"].get<std::string>());
                task.get()->setDescription(j["description"].get<std::string>());
                task.get()->setIsCompleted(j["completed"].get<bool>());
                task.get()->setPriority(j["priority"].get<int>());
                auto dueDateStr = j.value("dueDate", std::string(""));
                if (!dueDateStr.empty()) {
                    std::tm tm = {};
                    std::istringstream ss(dueDateStr);
                    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                    task.get()->setDueDate(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
                }
                
                tasks.push_back(task);
            } catch (const json::parse_error& e) {
                std::cerr << "Error parsing JSON line: " << line << std::endl;
                continue;
            }
        }
        
        file.close();
        return tasks;
    } catch (const std::exception& e) {
        std::cerr << "Error loading todo list: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool FileManager::saveTodoList(const std::vector<std::shared_ptr<Task>>& tasks) {
    try{
        std::ofstream file(todoFilePath_);
        
        if(!isValidFilePath(todoFilePath_)) {
            return false;
        }
        
        for (const auto& task : tasks) {
            json j;
            j["id"] = task->getId();
            j["name"] = task->getName();
            j["description"] = task->getDescription();
            j["completed"] = task->getIsCompleted();
            j["priority"] = task->getPriority();
            j["dueDate"] = task->getDueDateStr();
            
            file << j.dump() << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving todo list: " << e.what() << std::endl;
        return false;
    }   
}

