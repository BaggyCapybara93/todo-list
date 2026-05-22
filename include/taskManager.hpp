#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include "task.hpp"
#include "fileManager.hpp"
#include "settings.hpp"

class TaskManager{
    private:
        int nextId_;
        std::unordered_map<int, std::shared_ptr<Task>> tasks_;
        FileManager& fileManager_;

    public:
        TaskManager(FileManager& fm);
        
        ~TaskManager(); // Destructor for autosave
        
        void addTask(const std::string& name, const std::string& description, int priority, const std::chrono::system_clock::time_point& dueDate);
        
        void deleteTask(int id);
        
        const std::optional<std::shared_ptr<Task>> getTask(int id);
        
        const std::optional<std::shared_ptr<Task>> getTaskName(const std::string& name);
        
        void listTask(int id);

        bool completeTask(int id);

        const std::vector<std::shared_ptr<Task>> getPendingTasks();
        
        const std::vector<std::shared_ptr<Task>> getTasks();

        void listAllTasks();
        
        void loadTasks();
        
        void saveTasks();
        
        std::chrono::system_clock::time_point parseDueDate(const std::string& dueDateStr);
};      