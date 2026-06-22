#include "tagManager.hpp"

//Getters
std::shared_ptr<Tag> TagManager::getTagById(const int& id){
    try{
        for (auto& tag : *tags_.get()) {
                if (tag->getId() == id)
                    return tag;
            }
        return nullptr;
    } catch(const std::exception& e){
        std::cerr << "Error getting tag by id: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Tag> TagManager::getTagByName(const std::string& name){
    try{
        for (auto& tag : *tags_.get()) {
                if (tag->getName() == name)
                    return tag;
            }
        return nullptr;
    } catch(const std::exception& e){
        std::cerr << "Error getting tag by name: " << e.what() << std::endl;
        return nullptr;
    }
}

//Adder & Remover
bool TagManager::addTag(const std::string& name, const std::string& description){
    try{
        if (name.empty()) {
            std::cerr << "Error: Tag name cannot be empty." << std::endl;
            return false;
        }

        // Check if tag with same name already exists
        for (const auto& tag : *tags_.get()) {
            if (tag->getName() == name) {
                std::cerr << "Error: Tag with name '" << name << "' already exists." << std::endl;
                return false;
            }
        }

        auto newTag = std::make_shared<Tag>(nextId_++, name);
        newTag->addDescription(description);
        tags_.get()->push_back(newTag);
        std::cout << "Tag '" << name << "' added successfully with ID: " << nextId_ - 1 << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding tag: " << e.what() << std::endl;
        return false;
    }
}

bool TagManager::removeTag(const std::string& name, const int& id){
    try{
        bool removed = false;
        
        if (!name.empty()) {
            // Remove by name
            auto it = std::find_if(tags_.get()->begin(), tags_.get()->end(),
                [&name](const std::shared_ptr<Tag>& tag) {
                    return tag->getName() == name;
                });
            
            if (it != tags_.get()->end()) {
                std::cout << "Tag '" << name << "' removed successfully." << std::endl;
                tags_.get()->erase(it);
                removed = true;
            } else {
                std::cout << "Tag with name '" << name << "' not found." << std::endl;
            }
        } else if (id > 0) {
            // Remove by ID
            auto it = std::find_if(tags_.get()->begin(), tags_.get()->end(),
                [&id](const std::shared_ptr<Tag>& tag) {
                    return tag->getId() == id;
                });
            
            if (it != tags_.get()->end()) {
                std::cout << "Tag with ID " << id << " removed successfully." << std::endl;
                tags_.get()->erase(it);
                removed = true;
            } else {
                std::cout << "Tag with ID " << id << " not found." << std::endl;
            }
        } else {
            std::cout << "Error: Invalid tag parameters. Provide either tag name or tag ID." << std::endl;
        }
        
        return removed;
    } catch (const std::exception& e) {
        std::cerr << "Error removing tag: " << e.what() << std::endl;
        return false;
    }
}