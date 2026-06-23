#include "fileManager.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include "logger.hpp"
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

        if(!file.is_open()){
            Logger::log(Logger::LogLevel::WARNING, "Could not open todo list file.");
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

                // Tags will be loaded by TaskManager
                tasks.push_back(task);
            } catch (const json::parse_error& e) {
                Logger::log(Logger::LogLevel::ERROR, "Error parsing JSON line: " + line);
                continue;
            }
        }
        
        file.close();
        return tasks;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error loading todo list: " + std::string(e.what()));
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
            
            // Save tags for the task
            json tagsArray = json::array();
            for (const auto& tag : task->getTags()) {
                tagsArray.push_back(tag->getName());
            }
            j["tags"] = tagsArray;
            
            file << j.dump() << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error saving todo list: " + std::string(e.what()));
        return false;
    }   
}

