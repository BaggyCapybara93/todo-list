#include "utils.hpp"
#include <iostream>
#include <algorithm>

// Parse a due date string to chrono time_point
std::chrono::system_clock::time_point parseDueDate(const std::string& dueDateStr) {
    if (dueDateStr.empty()) {
        std::cerr << "Error: Empty date string provided." << std::endl;
        return std::chrono::system_clock::now();
    }
    
    std::tm timeVal = {};
    
    // Try parsing YYYY-MM-DD HH:MM:SS format first
    int parsed = std::sscanf(dueDateStr.c_str(), "%d-%d-%d %d:%d:%d",
                            &timeVal.tm_year, &timeVal.tm_mon, &timeVal.tm_mday,
                            &timeVal.tm_hour, &timeVal.tm_min, &timeVal.tm_sec);
    
    // If that fails, try YYYY-MM-DD format only
    if (parsed != 6) {
        parsed = std::sscanf(dueDateStr.c_str(), "%d-%d-%d",
                        &timeVal.tm_year, &timeVal.tm_mon, &timeVal.tm_mday);
    }
    
    if (parsed != 6 && parsed != 3) {
        std::cerr << "Error: Invalid due date format. Expected YYYY-MM-DD or YYYY-MM-DD HH:MM:SS, got: " 
                  << dueDateStr << std::endl;
        return std::chrono::system_clock::now();
    }
    
    // Set default time values if not provided
    if (parsed == 3) {
        timeVal.tm_hour = 0;
        timeVal.tm_min = 0;
        timeVal.tm_sec = 0;
    }
    
    //Subtract 1900 from tm_year as required by std::tm structure
    timeVal.tm_year -= 1900;
    
    // Validate date components (after adjusting tm_year)
    if (timeVal.tm_year < 0 || timeVal.tm_mon < 1 || timeVal.tm_mon > 12 ||
        timeVal.tm_mday < 1 || timeVal.tm_hour > 23 || timeVal.tm_min > 59 || timeVal.tm_sec > 59) {
        std::cerr << "Error: Invalid date values." << std::endl;
        return std::chrono::system_clock::now();
    }
    
    // Convert to time_t
    time_t currentTime = std::mktime(&timeVal);
    
    // Check if mktime succeeded
    if (currentTime == (-1)) {
        std::cerr << "Error: Failed to convert date to time_t." << std::endl;
        return std::chrono::system_clock::now();
    }
    
    // Convert to time_point
    return std::chrono::system_clock::from_time_t(currentTime);
}

// Validate a file path for safety
bool isValidFilePath(const std::string& path) {
    if (path.empty()) {
        std::cerr << "Error: Empty file path provided." << std::endl;
        return false;
    }
    
    // Check for path traversal attempts
    if (path.find("..") != std::string::npos) {
        std::cerr << "Error: Path traversal attempt detected." << std::endl;
        return false;
    }
    
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        std::cerr << "Error: Null byte detected in file path." << std::endl;
        return false;
    }
    
    return true;
}

//Sanitize a file path by removing potentially dangerous characters
std::string sanitizeFilePath(const std::string& path) {
    std::string sanitized = path;
    
    // Remove null bytes
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\0'), sanitized.end());
    
    // Replace path traversal sequences with empty string (effectively removing them)
    while (sanitized.find("..") != std::string::npos) {
        size_t pos = sanitized.find("..");
        sanitized.erase(pos, 2);
    }
    
    return sanitized;
}
