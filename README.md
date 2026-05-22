# Todo List Application

A C++ command-line todo list application with persistent storage.

## Features

- Add tasks with name, description, and priority (0-10)
- List pending and completed tasks
- Complete/delete tasks
- Export/Import tasks to/from JSON files
- Settings persistence (max tasks per file, autosave)

## Building

```bash
cd build
cmake ..
make
```

## Usage

```bash
./todo
```

### Menu Options

1. **Add Task** - Add a new task with name, description, and priority
2. **List Tasks** - View all pending tasks
3. **Complete Task** - Mark a task as complete by ID
4. **File Operations** - Export/Import tasks to/from files
5. **Exit** - Exit the application

## Settings

The application uses `settings.txt` for configuration:

```
max_tasks_per_file=1000
enable_autosave=true
```

- `max_tasks_per_file`: Maximum number of tasks per file (default: 1000)
- `enable_autosave`: Enable automatic saving on exit (default: true)

## Data Format

Tasks are stored in JSON format in `todo_list.txt`:

```json
{
"id":1,
"name":"Task Name",
"description":"Task description",
"isCompleted":false,
"priority":5,
"dueDate":"2026-01-01 00:00:00"
}
```

## Project Structure

```
todo/
├── CMakeLists.txt
├── include/
│   ├── consoleUI.hpp
│   ├── fileManager.hpp
│   ├── settings.hpp
│   ├── task.hpp
│   └── taskManager.hpp
├── src/
│   ├── consoleUI.cpp
│   ├── fileManager.cpp
│   ├── main.cpp
│   └── taskManager.cpp
└── build/
```

## License

MIT License
