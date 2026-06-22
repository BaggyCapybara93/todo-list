#pragma once

#include <memory>
#include <vector>

#include "tag.hpp"
#include "settings.hpp"

class TagManager{
    private:
        int nextId_ = 1;
        std::shared_ptr<std::vector<std::shared_ptr<Tag>>> tags_;
        std::shared_ptr<Settings> settings_;


    public: 
        TagManager(std::shared_ptr<Settings> settings)
            : settings_(settings) {}

        ~TagManager() = default;

        //Getters
        std::shared_ptr<Tag> getTagById(const int& id);
        std::shared_ptr<Tag> getTagByName(const std::string& name);

        //Setters
        bool setNextId(const int& id){ nextId_ = id; return true; }

        //Adder & Remover
        bool addTag(const std::string& name, const std::string& description = "");

        bool removeTag(const std::string& name = "", const int& id = -1);
        
};