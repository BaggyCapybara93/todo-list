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
        int id;
        std::string name;
        std::string description;
        bool isCompleted = false;
        int priority = 0;
        std::chrono::system_clock::time_point dueDate;
        std::unordered_set<std::shared_ptr<Tag>> tags_;
        RepeatInterval repeatInterval = RepeatInterval::NEVER;
        std::chrono::system_clock::time_point nextDueDate;

    public:
        Task() : id(0), name(""), description(""), isCompleted(false), priority(0), 
                  dueDate(std::chrono::system_clock::time_point()), 
                  nextDueDate(std::chrono::system_clock::time_point()) {}

        Task(int id, const std::string& name, const std::string& description, bool isCompleted, 
             int priority, const std::chrono::system_clock::time_point& dueDate, 
             RepeatInterval repeatInterval = RepeatInterval::NEVER)
            : id(id), name(name), description(description), isCompleted(isCompleted), 
              priority(priority), dueDate(dueDate), repeatInterval(repeatInterval),
              nextDueDate(std::chrono::system_clock::time_point()) {}
        
        //Getters
        const int& getId() const { return id; }
        const std::string& getName() const { return name; }
        const std::string& getDescription() const { return description; }
        const bool& getIsCompleted() const { return isCompleted; }
        const int& getPriority() const { return priority; }
        const std::chrono::system_clock::time_point& getDueDate() const { return dueDate; }
        const std::unordered_set<std::shared_ptr<Tag>>& getTags() const { return tags_; }
        const RepeatInterval& getRepeatInterval() const { return repeatInterval; }
        const std::chrono::system_clock::time_point& getNextDueDate() const { return nextDueDate; }

        //Setters
        void setId(int newId) { id = newId; }
        void setName(const std::string& newName) { name = newName; }
        void setDescription(const std::string& newDescription) { description = newDescription; }
        void setIsCompleted(bool completed) { isCompleted = completed; }
        void setPriority(int newPriority) { priority = newPriority; }
        void setDueDate(const std::chrono::system_clock::time_point& newDueDate) { dueDate = newDueDate; }
        void setRepeatInterval(RepeatInterval interval) { repeatInterval = interval; }
        void setNextDueDate(const std::chrono::system_clock::time_point& newNextDueDate) { nextDueDate = newNextDueDate; }
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
            return "[" + std::to_string(id) + "] " + description + 
                " (Priority: " + std::to_string(priority) + 
                ") - " + (isCompleted ? "DONE" : "PENDING") + 
                " [" + dueDateStr + "]" + repeatInfo + tagStr;
        }

        std::string getDueDateStr() const {
            if (dueDate == std::chrono::system_clock::time_point()) {
                return "";
            }
            auto time_t_val = std::chrono::system_clock::to_time_t(dueDate);
            std::tm* tm = std::gmtime(&time_t_val);
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buffer);
        }

        std::string getNextDueDateStr() const {
            if (nextDueDate == std::chrono::system_clock::time_point()) {
                return "";
            }
            auto time_t_val = std::chrono::system_clock::to_time_t(nextDueDate);
            std::tm* tm = std::gmtime(&time_t_val);
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buffer);
        }

        std::string getRepeatIntervalStr() const {
            switch (repeatInterval) {
                case RepeatInterval::NEVER: return "NEVER";
                case RepeatInterval::DAILY: return "DAILY";
                case RepeatInterval::WEEKLY: return "WEEKLY";
                case RepeatInterval::MONTHLY: return "MONTHLY";
                case RepeatInterval::YEARLY: return "YEARLY";
                default: return "NEVER";
            }
        }

        void calculateNextDueDate() {
            if (repeatInterval == RepeatInterval::NEVER || isCompleted) {
                nextDueDate = std::chrono::system_clock::time_point();
                return;
            }

            auto now = std::chrono::system_clock::now();
            if (now >= nextDueDate) {
                nextDueDate = dueDate;
            }
        }
};