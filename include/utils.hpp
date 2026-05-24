#pragma once

#include <string>
#include <chrono>
#include <ctime>

std::chrono::system_clock::time_point parseDueDate(const std::string& dueDateStr);

bool isValidFilePath(const std::string& path);

std::string sanitizeFilePath(const std::string& path);
