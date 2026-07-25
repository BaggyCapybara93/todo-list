#include "fileManager.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include "logger.hpp"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <ctime>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using json = nlohmann::json;

FileManager::FileManager(const std::string& todoFilePath) : todoFilePath_(todoFilePath) {}

bool FileManager::isValidFilePath(const std::string& filePath) const {
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        return false;
    }
    
    // Check if it's a regular file
    if (!S_ISREG(fileStat.st_mode)) {
        return false;
    }
    
    return true;
}

bool FileManager::isFileReadable(const std::string& filePath) const {
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        return false;
    }
    
    // Check read permission for owner, group, or others
    if (fileStat.st_mode & S_IRUSR) return true;
    if (fileStat.st_mode & S_IRGRP) return true;
    if (fileStat.st_mode & S_IROTH) return true;
    
    return false;
}

bool FileManager::isFileWritable(const std::string& filePath) const {
    struct stat dirStat;
    std::string dirPath = filePath;
    size_t lastSlash = filePath.rfind('/');
    
    if (lastSlash != std::string::npos) {
        dirPath = filePath.substr(0, lastSlash);
    }
    
    // Check if directory exists and is writable
    if (stat(dirPath.c_str(), &dirStat) != 0) {
        return false;
    }
    
    if (!S_ISDIR(dirStat.st_mode)) {
        return false;
    }
    
    // Check write permission for directory
    if (dirStat.st_mode & S_IWUSR) return true;
    if (dirStat.st_mode & S_IWGRP) return true;
    if (dirStat.st_mode & S_IWOTH) return true;
    
    return false;
}

std::string FileManager::escapeJsonString(const std::string& str) const {
    return nlohmann::detail::escape(str);
}

std::optional<std::vector<std::shared_ptr<Task>>> FileManager::loadTodoList() {
    try {
        std::vector<std::shared_ptr<Task>> tasks;
        
        // Check file permissions before attempting to read
        if (!isFileReadable(todoFilePath_)) {
            Logger::log(Logger::LogLevel::WARNING, "Todo list file is not readable. Check file permissions.");
            return std::nullopt;
        }
        
        std::ifstream file(todoFilePath_);

        if (!isValidFilePath(todoFilePath_)) {
            return std::nullopt;
        }

        if (!file.is_open()) {
            Logger::log(Logger::LogLevel::WARNING, "Could not open todo list file.");
            return std::nullopt;
        }

        // Temporary maps to store relationships by ID
        std::unordered_map<int, std::vector<int>> subtaskMap;
        std::unordered_map<int, std::vector<int>> dependencyMap;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            try {
                json j = json::parse(line);
                auto task = std::make_shared<Task>();

                // Basic fields
                task->setId(j["id"].get<int>());
                task->setName(j["name"].get<std::string>());
                task->setDescription(j["description"].get<std::string>());
                task->setIsCompleted(j["completed"].get<bool>());
                task->setPriority(j["priority"].get<int>());

                // Due date
                auto dueDateStr = j.value("dueDate", "");
                if (!dueDateStr.empty()) {
                    std::tm tm = {};
                    std::istringstream ss(dueDateStr);
                    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                    task->setDueDate(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
                }

                // Subtasks (store IDs temporarily)
                if (j.contains("subtasks")) {
                    subtaskMap[task->getId()] = j["subtasks"].get<std::vector<int>>();
                }

                // Dependencies (store IDs temporarily)
                if (j.contains("dependencies")) {
                    dependencyMap[task->getId()] = j["dependencies"].get<std::vector<int>>();
                }

                tasks.push_back(task);

            } catch (const json::parse_error& e) {
                Logger::log(Logger::LogLevel::ERROR, "Error parsing JSON line: " + line);
                continue;
            }
        }

        file.close();

        // Check write permission before saving
        if (!isFileWritable(todoFilePath_)) {
            Logger::log(Logger::LogLevel::WARNING, "Todo list file is not writable. Check file permissions.");
            return std::nullopt;
        }

        // Build lookup table for ID → Task*
        std::unordered_map<int, std::shared_ptr<Task>> lookup;
        for (auto& t : tasks) {
            lookup[t->getId()] = t;
        }

        // Resolve subtasks
        for (auto& [taskId, subIds] : subtaskMap) {
            auto parent = lookup[taskId];
            for (int sid : subIds) {
                if (lookup.count(sid)) {
                    parent->addSubtask(lookup[sid]);
                }
            }
        }

        // Resolve dependencies
        for (auto& [taskId, depIds] : dependencyMap) {
            auto parent = lookup[taskId];
            for (int did : depIds) {
                if (lookup.count(did)) {
                    parent->addDependency(lookup[did]);
                }
            }
        }

        return tasks;

    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error loading todo list: " + std::string(e.what()));
        return std::nullopt;
    }
}

bool FileManager::saveTodoList(const std::vector<std::shared_ptr<Task>>& tasks) {
    try {
        // Check file permissions before attempting to write
        if (!isFileWritable(todoFilePath_)) {
            Logger::log(Logger::LogLevel::ERROR, "Todo list file is not writable. Check file permissions.");
            return false;
        }
        
        std::ofstream file(todoFilePath_);

        if (!isValidFilePath(todoFilePath_)) {
            return false;
        }

        if (!file.is_open()) {
            Logger::log(Logger::LogLevel::ERROR, "Could not open todo list file for writing.");
            return false;
        }

        for (const auto& task : tasks) {
            json j;

            // Basic fields
            j["id"] = task->getId();
            j["name"] = task->getName();
            j["description"] = task->getDescription();
            j["completed"] = task->getIsCompleted();
            j["priority"] = task->getPriority();
            j["dueDate"] = task->getDueDateStr();

            // Repeat interval
            j["repeatInterval"] = repeatIntervalToString(task->getRepeatInterval());

            // Tags
            json tagsArray = json::array();
            for (const auto& tag : task->getTags()) {
                tagsArray.push_back(tag->getName());
            }
            j["tags"] = tagsArray;

            // Subtasks (store IDs only)
            json subArray = json::array();
            for (const auto& sub : task->getSubtasks()) {
                subArray.push_back(sub->getId());
            }
            j["subtasks"] = subArray;

            // Dependencies (store IDs only)
            json depArray = json::array();
            for (const auto& dep : task->getDependencies()) {
                depArray.push_back(dep->getId());
            }
            j["dependencies"] = depArray;

            // Write JSON line
            file << j.dump() << "\n";
        }

        file.close();
        return true;

    } catch (const std::exception& e) {
        Logger::log(Logger::LogLevel::ERROR, "Error saving todo list: " + std::string(e.what()));
        return false;
    }
}


