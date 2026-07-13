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
    try {
        std::vector<std::shared_ptr<Task>> tasks;
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


