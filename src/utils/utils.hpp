#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include "task/task.hpp"

std::chrono::system_clock::time_point parseDueDate(const std::string& dueDateStr);

bool isValidFilePath(const std::string& path);

std::string sanitizeFilePath(const std::string& path);

// Repeat interval parsing and conversion
RepeatInterval parseRepeatInterval(const std::string& intervalStr);
std::string repeatIntervalToString(RepeatInterval interval);
