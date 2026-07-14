#include "task/manager.hpp"

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByDueDate(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getDueDate() != std::chrono::system_clock::time_point()) {
            if (pair.second->getDueDate() >= minDueDate && pair.second->getDueDate() <= maxDueDate) {
                filtered->push_back(pair.second);
            }
        }
    }
    return filtered;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByPriority(int minPriority, int maxPriority) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getPriority() >= minPriority && pair.second->getPriority() <= maxPriority) {
            filtered->push_back(pair.second);
        }
    }
    return filtered;
}

const std::shared_ptr<std::vector<std::shared_ptr<Task>>> TaskManager::filterTasksByDueDateAndPriority(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate, int minPriority, int maxPriority) {
    auto filtered = std::make_shared<std::vector<std::shared_ptr<Task>>>();
    for (const auto& pair : tasks_) {
        if (!pair.second->getIsCompleted() && pair.second->getDueDate() != std::chrono::system_clock::time_point()) {
            if (pair.second->getDueDate() >= minDueDate && pair.second->getDueDate() <= maxDueDate &&
                pair.second->getPriority() >= minPriority && pair.second->getPriority() <= maxPriority) {
                filtered->push_back(pair.second);
            }
        }
    }
    return filtered;
}