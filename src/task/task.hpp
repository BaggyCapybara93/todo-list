#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <memory>
#include <unordered_set>

#include "tag/tag.hpp"

enum class RepeatInterval {
    NEVER,
    DAILY,
    WEEKLY,
    MONTHLY,
    YEARLY
};
class Task{
    private:
        int id_;
        std::string name_;
        std::string description_;
        bool isCompleted_ = false;
        int priority_ = 0;
        std::chrono::system_clock::time_point dueDate_;
        std::unordered_set<std::shared_ptr<Tag>> tags_;
        RepeatInterval repeatInterval_ = RepeatInterval::NEVER;
        std::chrono::system_clock::time_point nextDueDate_;

        std::unordered_set<std::shared_ptr<Task>> subtasks_;
        std::unordered_set<std::shared_ptr<Task>> dependencies_;
    public:
        Task() : id_(0), name_(""), description_(""), isCompleted_(false), priority_(0), 
                  dueDate_(std::chrono::system_clock::time_point()), 
                  nextDueDate_(std::chrono::system_clock::time_point()) {}

        Task(int id, const std::string& name, const std::string& description, bool isCompleted, 
             int priority, const std::chrono::system_clock::time_point& dueDate, 
             RepeatInterval repeatInterval = RepeatInterval::NEVER)
            : id_(id), name_(name), description_(description), isCompleted_(isCompleted), 
              priority_(priority), dueDate_(dueDate), repeatInterval_(repeatInterval),
              nextDueDate_(std::chrono::system_clock::time_point()) {}
        
        //Getters
        const int& getId() const { return id_; }
        const std::string& getName() const { return name_; }
        const std::string& getDescription() const { return description_; }
        const bool& getIsCompleted() const { return isCompleted_; }
        const int& getPriority() const { return priority_; }
        const std::chrono::system_clock::time_point& getDueDate() const { return dueDate_; }
        const std::unordered_set<std::shared_ptr<Tag>>& getTags() const { return tags_; }
        const RepeatInterval& getRepeatInterval() const { return repeatInterval_; }
        const std::chrono::system_clock::time_point& getNextDueDate() const { return nextDueDate_; }

        //Setters
        void setId(int newId) { id_ = newId; }
        void setName(const std::string& newName) { name_ = newName; }
        void setDescription(const std::string& newDescription) { description_ = newDescription; }
        void setIsCompleted(bool completed) { isCompleted_ = completed; }
        void setPriority(int newPriority) { priority_ = newPriority; }
        void setDueDate(const std::chrono::system_clock::time_point& newDueDate) { dueDate_ = newDueDate; }
        void setRepeatInterval(RepeatInterval interval) { repeatInterval_ = interval; }
        void setNextDueDate(const std::chrono::system_clock::time_point& newNextDueDate) { nextDueDate_ = newNextDueDate; }
        void addTag(const std::shared_ptr<Tag>& tag) { tags_.insert(tag); }
        void removeTag(const std::shared_ptr<Tag>& tag) { 
            tags_.erase(tag);
        }

        //Helper Functions
        std::string toString() const{
            auto dueDateStr = getDueDateStr();
            auto repeatStr = getRepeatIntervalStr();
            std::string tagStr = "";
            for (const auto& tag : tags_) {
                tagStr += " #" + tag->getName();
            }
            std::string repeatInfo = (repeatStr != "NEVER") ? " (Repeat: " + repeatStr + ")" : "";
            return "[" + std::to_string(id_) + "] " + description_ + 
                " (Priority: " + std::to_string(priority_) + 
                ") - " + (isCompleted_ ? "DONE" : "PENDING") + 
                " [" + dueDateStr + "]" + repeatInfo + tagStr;
        }

        std::string getDueDateStr() const {
            if (dueDate_ == std::chrono::system_clock::time_point()) {
                return "";
            }
            auto time_t_val = std::chrono::system_clock::to_time_t(dueDate_);
            std::tm* tm = std::gmtime(&time_t_val);
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buffer);
        }

        std::string getNextDueDateStr() const {
            if (nextDueDate_ == std::chrono::system_clock::time_point()) {
                return "";
            }
            auto time_t_val = std::chrono::system_clock::to_time_t(nextDueDate_);
            std::tm* tm = std::gmtime(&time_t_val);
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buffer);
        }

        std::string getRepeatIntervalStr() const {
            switch (repeatInterval_) {
                case RepeatInterval::NEVER: return "NEVER";
                case RepeatInterval::DAILY: return "DAILY";
                case RepeatInterval::WEEKLY: return "WEEKLY";
                case RepeatInterval::MONTHLY: return "MONTHLY";
                case RepeatInterval::YEARLY: return "YEARLY";
                default: return "NEVER";
            }
        }

        void calculateNextDueDate() {
            if (repeatInterval_ == RepeatInterval::NEVER || isCompleted_) {
                nextDueDate_ = std::chrono::system_clock::time_point();
                return;
            }

            auto now = std::chrono::system_clock::now();
            if (now >= nextDueDate_) {
                nextDueDate_ = dueDate_;
            }
        }


        //Subtask and Dependency Management
        std::unordered_set<std::shared_ptr<Task>> getSubtasks() const { return subtasks_; }

        void addSubtask(const std::shared_ptr<Task>& subtask){
            subtasks_.insert(subtask);
        }

        void removeSubtask(const std::shared_ptr<Task>& subtask){
            subtasks_.erase(subtask);
        }

        std::shared_ptr<Task> getSubtaskById(int subtaskId) const{
            for (const auto& subtask : subtasks_) {
                if (subtask->getId() == subtaskId) {
                    return subtask;
                }
            }
            return nullptr;
        }

        std::shared_ptr<Task> getSubtaskByName(const std::string& subtaskName) const{
            for (const auto& subtask : subtasks_) {
                if (subtask->getName() == subtaskName) {
                    return subtask;
                }
            }
            return nullptr;
        }

        std::unordered_set<std::shared_ptr<Task>> getDependencies() const { return dependencies_; }

        void addDependency(const std::shared_ptr<Task>& dependency){
            dependencies_.insert(dependency);
        }

        void removeDependency(const std::shared_ptr<Task>& dependency){
            dependencies_.erase(dependency);
        }
        
        std::shared_ptr<Task> getDependencyById(int dependencyId) const{
            for (const auto& dependency : dependencies_) {
                if (dependency->getId() == dependencyId) {
                    return dependency;
                }
            }
            return nullptr;
        }

        std::shared_ptr<Task> getDependencyByName(const std::string& dependencyName) const{
            for (const auto& dependency : dependencies_) {
                if (dependency->getName() == dependencyName) {
                    return dependency;
                }
            }
            return nullptr;
        }
};