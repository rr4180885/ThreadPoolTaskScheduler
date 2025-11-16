# Advanced Thread Pool Task Scheduler

A professional C++ Thread Pool implementation that manages multiple worker threads to execute tasks concurrently. This project demonstrates advanced multi-threading concepts, synchronization mechanisms, dynamic task scheduling, and is designed for resume/portfolio showcasing.

## 🌟 Key Features

- ✅ **Dynamic Configuration**: User can specify number of worker threads and tasks at runtime
- ✅ **Random Task Generation**: Tasks are generated with random durations for realistic workload simulation
- ✅ **Thread-Safe Queue**: Utilizes `std::mutex` and `std::condition_variable` for safe concurrent access
- ✅ **Generic Task Support**: Submit any function with any return type and arguments
- ✅ **Future-Based Results**: Asynchronous result retrieval using `std::future` and `std::packaged_task`
- ✅ **Graceful Shutdown**: Ensures all tasks complete before terminating threads
- ✅ **Exception Handling**: Robust error handling and reporting
- ✅ **Professional Code Structure**: Separated header file, extensive inline documentation
- ✅ **RAII Pattern**: Automatic resource management and cleanup

## 📁 Project Structure

```
.
├── ThreadPool.h       # ThreadPool class header with complete implementation
├── main.cpp           # Main program with user input and dynamic task generation
├── test_auto.cpp      # Automated test version (no user input required)
├── README.md          # This file - comprehensive documentation
├── .gitignore         # Git ignore rules for C++ projects
└── replit.md          # Technical architecture documentation
```

## 🛠️ Technical Requirements

- **C++ Standard**: C++17 or later
- **Threading Library**: pthread
- **Compiler**: g++ 7.0+ or clang++ 5.0+
- **Platform**: Linux, macOS, Windows (with MinGW/WSL)

## 🚀 Quick Start

### Compilation

```bash
# Compile the main interactive program
g++ -std=c++17 -pthread main.cpp -o threadpool

# Compile the automated test version
g++ -std=c++17 -pthread test_auto.cpp -o test_auto
```

### Running the Program

#### Interactive Mode (User Input):
```bash
./threadpool
```

You will be prompted to enter:
1. Number of worker threads (1-16)
2. Number of tasks to execute (1-100)

#### Automated Test Mode:
```bash
./test_auto
```

Runs a predefined test with 4 threads and 10 tasks.

## 📊 Example Output

```
========================================
  THREAD POOL TASK SCHEDULER
  Dynamic Concurrent Task Execution
========================================

Enter number of worker threads (1-16): 4
Enter number of tasks to execute (1-100): 10

Creating thread pool with 4 worker threads...
Thread pool created successfully!

Submitting 10 tasks to the thread pool...

Tasks submitted! Pending in queue: 6

--- Tasks Executing Concurrently ---

[Task 1] Started - Duration: 1.75s
[Task 2] Started - Duration: 0.69s
[Task 3] Started - Duration: 1.16s
[Task 4] Started - Duration: 1.87s
[Task 2] Task 2 completed successfully after 0.69s
[Task 5] Started - Duration: 1.21s
...

--- Collecting Task Results ---

Result 1: Task 1 completed successfully after 1.75s
Result 2: Task 2 completed successfully after 0.69s
...

========================================
  EXECUTION SUMMARY
========================================
Worker Threads: 4
Tasks Executed: 10
Status: All tasks completed successfully
========================================
```

## 🏗️ Architecture Overview

### ThreadPool Class (`ThreadPool.h`)

The core component that manages concurrent task execution:

#### Public Methods:
- **`ThreadPool(size_t num_threads)`**: Constructor that initializes worker threads
- **`submit(F&& f, Args&&... args)`**: Template method to submit any callable for execution
- **`get_queue_size()`**: Returns number of pending tasks in queue
- **`shutdown()`**: Gracefully stops all worker threads
- **`~ThreadPool()`**: Destructor that ensures proper cleanup (RAII)

#### Private Members:
- **`workers`**: Vector of worker thread objects
- **`tasks`**: Queue storing pending tasks
- **`queue_mutex`**: Mutex protecting the task queue
- **`condition`**: Condition variable for thread synchronization
- **`stop`**: Flag indicating shutdown state

#### Private Methods:
- **`worker_loop()`**: Main loop executed by each worker thread

### Main Program (`main.cpp`)

Demonstrates the ThreadPool with dynamic user input:

#### Key Functions:
- **`execute_task(int task_id, double duration)`**: Simulates work and returns result
- **`generate_random_duration(double min, double max)`**: Creates random task durations
- **`get_user_input(prompt, min, max)`**: Validates and retrieves user input
- **`display_header()`**: Shows program title and information
- **`display_summary(threads, tasks)`**: Presents execution statistics

## 🔍 Implementation Details

### Thread Safety Mechanisms

1. **Mutex Locking**: Protects shared task queue from race conditions
2. **Condition Variables**: Efficiently wakes threads when tasks available
3. **RAII Locks**: `std::unique_lock` ensures automatic lock release
4. **Atomic Operations**: Stop flag prevents submission to shutdown pool

### Task Execution Flow

1. User submits task via `submit()` method
2. Task wrapped in `std::packaged_task` and added to queue
3. Worker threads wait on condition variable
4. When task available, thread acquires lock, pops task, releases lock
5. Task executes outside critical section (allows concurrency)
6. Result stored in `std::future` for later retrieval
7. Main thread collects results via `future.get()`

### Random Task Generation

- Uses `std::random_device` for true random seeding
- Employs `std::mt19937` Mersenne Twister generator
- `std::uniform_real_distribution` for duration between 0.5-3.0 seconds
- Each task gets unique ID and random execution time

### Input Validation

- Validates numeric input (rejects characters/invalid types)
- Enforces reasonable ranges (1-16 threads, 1-100 tasks)
- Provides clear error messages and re-prompts on invalid input
- Clears input buffer to prevent infinite loops

## 📈 Performance Characteristics

- **Concurrency Level**: Configurable (1-16 worker threads)
- **Task Queue**: O(1) push/pop operations
- **Thread Synchronization**: Minimal lock contention (tasks execute outside locks)
- **Memory Efficiency**: Tasks use shared_ptr for safe memory management
- **Scalability**: Performance scales with number of cores

## 🎓 Concepts Demonstrated

| Concept | Implementation |
|---------|----------------|
| **Multi-threading** | std::thread, worker pool pattern |
| **Synchronization** | std::mutex, std::condition_variable |
| **Task Queue** | Producer-consumer pattern |
| **Futures** | std::future, std::packaged_task |
| **Template Programming** | Generic function submission |
| **RAII** | Automatic resource cleanup |
| **Exception Safety** | try-catch blocks, exception propagation |
| **Random Generation** | std::random_device, std::mt19937 |
| **Input Validation** | Error handling, range checking |

## 💼 Resume/Portfolio Highlights

This project demonstrates:
- Advanced C++ knowledge (C++17 features, templates, STL)
- Multi-threading and concurrent programming expertise
- Thread-safe design patterns (producer-consumer)
- Professional code organization (separation of concerns)
- Comprehensive inline documentation (every line explained)
- User experience design (input validation, clear feedback)
- Software engineering best practices (RAII, exception safety)
- Testing methodology (automated test suite)

## 📝 Code Comments

Every line in this project includes detailed explanatory comments:
- **What** the code does
- **Why** it's necessary
- **How** it works
- **When** it executes

This makes the codebase excellent for:
- Learning multi-threading concepts
- Code review demonstrations
- Technical interviews
- Teaching/mentoring

## 🔧 Customization

### Adjust Thread Count Range
Edit `main.cpp` line with `get_user_input`:
```cpp
int num_threads = get_user_input("Enter threads: ", 1, 32);  // Changed max to 32
```

### Modify Task Duration Range
Edit `main.cpp` in task submission loop:
```cpp
double duration = generate_random_duration(1.0, 5.0);  // 1-5 seconds instead of 0.5-3
```

### Change Task Function
Replace `execute_task` with your own function:
```cpp
int my_computation(int value) {
    // Your logic here
    return value * 2;
}

// Submit it
futures.push_back(pool.submit(my_computation, 42));
```

## 🐛 Error Handling

The program handles:
- Invalid user input (non-numeric, out of range)
- Task exceptions (caught and reported per-task)
- Submission to stopped pool (throws runtime_error)
- Resource cleanup on abnormal termination

## 📜 License

This is an educational/portfolio project. Free to use for learning and demonstration purposes.

## 👨‍💻 Author

Created as a professional demonstration of C++ multi-threading expertise.  
Perfect for technical resumes, portfolios, and interview discussions.

## 🙏 Acknowledgments

- C++ Standard Library threading components
- Modern C++ design patterns and best practices
- Producer-consumer pattern implementations

---

**⭐ If you find this project useful for learning or your portfolio, feel free to star it!**
