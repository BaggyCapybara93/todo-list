#pragma once
#include <iostream>
#include <vector>

class Tag{
    private:
        int id_;
        std::string name_;
        std::string descriptions_;

    public:
        Tag(int id, const std::string& name)
            : id_(id), name_(name) {}

        const int& getId() const {
            return id_;
        }

        const std::string& getName() const {
            return name_;
        }

        void addDescription(const std::string& description) {
            descriptions_ = description;
        }

        const std::string& getDescription() const {
            return descriptions_;
        }

};