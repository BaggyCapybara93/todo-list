#pragma once
#include <unordered_map>
#include <vector>
#include <optional>
#include "task.hpp"
#include "file_manager/fileManager.hpp"
#include "tag/manager.hpp"
#include "settings.hpp"

class TaskManager{
    private:
        int nextId_;
        std::unordered_map<int, std::shared_ptr<Task>> tasks_;
        std::shared_ptr<FileManager> fileManager_;
        std::shared_ptr<TagManager> tagManager_;
        std::shared_ptr<Settings> settings_;

    public:
        TaskManager(std::shared_ptr<FileManager> fm, std::shared_ptr<TagManager> tm, std::shared_ptr<Settings> s);
        
        ~TaskManager(); // Destructor for autosave
        
        int addTask(const std::string& name, const std::string& description, int priority, const std::chrono::system_clock::time_point& dueDate);
        
        void deleteTask(int id);
        
        const std::optional<std::shared_ptr<Task>> getTask(int id);
        
        const std::optional<std::shared_ptr<Task>> getTaskName(const std::string& name);
        
        void listTask(int id);

        bool completeTask(int id);

        const std::shared_ptr<std::vector<std::shared_ptr<Task>>> getPendingTasks();
        
        const std::shared_ptr<std::vector<std::shared_ptr<Task>>> getTasks();

        // Filter tasks by due date and priority
        /**
         * @brief Filter tasks based on due date
         * @param minDueDate Minimum due date
         * @param maxDueDate Maximum due date
         * @return Vector of filtered tasks
         */
        const std::shared_ptr<std::vector<std::shared_ptr<Task>>> filterTasksByDueDate(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate);
        
        /**
         * @brief Filter tasks based on priority
         * @param minPriority Minimum priority
         * @param maxPriority Maximum priority
         * @return Vector of filtered tasks
         */
        const std::shared_ptr<std::vector<std::shared_ptr<Task>>> filterTasksByPriority(int minPriority, int maxPriority);
        
        /**
         * @brief Filter tasks based on priority and due date
         * @param minDueDate Minimum due date
         * @param maxDueDate Maximum due date
         * @param minPriority Minimum priority
         * @param maxPriority Maximum priority
         * @return Vector of filtered tasks
         */
        const std::shared_ptr<std::vector<std::shared_ptr<Task>>>filterTasksByDueDateAndPriority(const std::chrono::system_clock::time_point& minDueDate, const std::chrono::system_clock::time_point& maxDueDate, int minPriority, int maxPriority);

        // Tag-related methods
        bool addTag(const std::string& name, const std::string& description = "");
        bool removeTag(const std::string& name = "", const int& id = -1);
        std::shared_ptr<Tag> getTagById(const int& id);
        std::shared_ptr<Tag> getTagByName(const std::string& name);
        bool addTagToTask(int taskId, const std::string& tagName);
        bool removeTagFromTask(int taskId, const std::string& tagName);

        // Repeat task methods
        bool setRepeatTask(int taskId, const std::string& intervalStr);
        RepeatInterval getRepeatInterval(int taskId);
        void calculateNextDueDates();

        void listAllTasks();
        
        void loadTasks();
        
        void saveTasks();
        
        //Subtask and Dependency Management
        std::unordered_set<std::shared_ptr<Task>> getSubtasks(int taskId);
        void addSubtask(int taskId, const std::shared_ptr<Task>& subtask);
        void removeSubtask(int taskId, const std::shared_ptr<Task>& subtask);
        std::shared_ptr<Task> getSubtaskById(int taskId, int subtaskId);
        std::shared_ptr<Task> getSubtaskByName(int taskId, const std::string& subtaskName);
        bool hasSubtask(int taskId);

        std::unordered_set<std::shared_ptr<Task>> getDependencies(int taskId);
        void addDependency(int taskId, const std::shared_ptr<Task>& dependency);
        void removeDependency(int taskId, const std::shared_ptr<Task>& dependency);
        std::shared_ptr<Task> getDependencyById(int taskId, int dependencyId);
        std::shared_ptr<Task> getDependencyByName(int taskId, const std::string& dependencyName);
        bool hasDependency(int taskId);

        //Helper Functions
        std::chrono::system_clock::time_point parseDueDate(const std::string& dueDateStr);
};      