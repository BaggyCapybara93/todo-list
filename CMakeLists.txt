cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(todo)

# Enable all warnings and treat them as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wcast-align -Wformat=2 -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual")

set(TODO_SOURCES 
    src/main.cpp 
    src/task/manager.cpp
    src/task/getters.cpp
    src/task/filter.cpp
    src/file_manager/fileManager.cpp
    src/settings.cpp
    src/utils/utils.cpp
    src/logger.cpp
    src/tag/manager.cpp
    src/cli/cli_handler.cpp
    src/cli/cli.cpp
    src/parse_arguments/parser.cpp
    src/parse_arguments/arguments.cpp
)

add_executable(todo ${TODO_SOURCES})

target_include_directories(todo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(Boost 1.71.0 CONFIG COMPONENTS program_options)

find_package(nlohmann_json REQUIRED)

target_link_libraries(todo PRIVATE Boost::program_options nlohmann_json::nlohmann_json)
