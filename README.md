# Todo List Application

A C++ command-line todo list application with persistent storage.

## Features

- Add tasks with name, description, priority (0-10), and due date
- List pending and completed tasks with filtering options
- Complete tasks by ID
- Manage tags for organizing tasks (add and remove tags)
- Export/Import tasks to/from text files
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

1. **Add Task** - Add a new task with name, description, priority, and due date
2. **List Tasks** - View all pending tasks (with optional filtering)
3. **Complete Task** - Mark a task as complete by ID
4. **Add Tag** - Create a new tag for organizing tasks
5. **Remove Tag** - Delete an existing tag
6. **File Operations** - Export/Import tasks to/from files
7. **Exit** - Exit the application

## Settings

The application uses `settings.txt` for configuration:

```
max_tasks_per_file=1000
enable_autosave=true
```

- `max_tasks_per_file`: Maximum number of tasks per file (default: 1000)
- `enable_autosave`: Enable automatic saving on exit (default: true)

## Data Format

Tasks are stored in text format in `.todo_list.txt`:

```
id:1|name:Task Name|description:Task description|isCompleted:false|priority:5|dueDate:2026-01-01 00:00:00
```

Tags are stored in `.tags.txt`:

```
id:1|name:Work|description:Work-related tasks
```

## License

MIT License
