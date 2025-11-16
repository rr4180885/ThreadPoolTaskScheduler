// ThreadPool.h
// Header file for Thread Pool Task Scheduler
// This file contains the ThreadPool class declaration and implementation

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <vector>
#include <stdexcept>

// ThreadPool class manages a pool of worker threads that execute tasks concurrently
class ThreadPool {
private:
    // Vector to store all worker thread objects
    std::vector<std::thread> workers;
    
    // Queue to hold tasks waiting to be executed
    std::queue<std::function<void()>> tasks;
    
    // Mutex to protect the task queue from race conditions
    std::mutex queue_mutex;
    
    // Condition variable to notify worker threads when tasks are available
    std::condition_variable condition;
    
    // Flag to signal when the thread pool should stop accepting new tasks
    bool stop;

    // Helper function: Predicate for condition variable
    // Checks if worker thread should wake up (task available or stop requested)
    // Returns: true if thread should wake up, false if it should keep waiting
    bool should_wake_up() {
        // Return true if stop flag is set OR queue has tasks
        // This is checked while holding the lock in worker_loop
        return stop || !tasks.empty();
    }

    // Helper function: Task execution wrapper
    // Executes a packaged_task when called
    // Parameter: task_ptr - shared pointer to the packaged_task to execute
    template<typename ReturnType>
    static void execute_packaged_task(std::shared_ptr<std::packaged_task<ReturnType()>> task_ptr) {
        // Dereference the shared_ptr and execute the packaged_task
        // The () operator on packaged_task runs the wrapped function
        (*task_ptr)();
    }

    // Worker thread main loop function
    // Each worker thread continuously executes this function
    void worker_loop() {
        // Infinite loop - worker keeps running until stop flag is set
        while (true) {
            // Variable to hold the task fetched from queue
            std::function<void()> task;
            
            // Critical section - accessing shared task queue
            {
                // Acquire lock on the queue mutex
                std::unique_lock<std::mutex> lock(queue_mutex);
                
                // Wait until there's a task in queue or stop flag is set
                // This releases the lock and puts thread to sleep
                // Thread wakes up when condition variable is notified
                // We use a while loop to manually check the predicate (no lambda)
                while (!should_wake_up()) {
                    // Wait releases the lock and sleeps until notified
                    condition.wait(lock);
                    // After waking, lock is reacquired automatically
                    // Loop continues to check predicate again
                }
                
                // If stop flag is set and no tasks remain, exit the worker thread
                if (stop && tasks.empty()) {
                    return;
                }
                
                // Fetch the task from front of queue
                task = std::move(tasks.front());
                
                // Remove the task from queue
                tasks.pop();
                
                // Lock is automatically released when this block ends
            }
            
            // Execute the task outside the critical section
            // This allows other threads to access the queue while task executes
            task();
        }
    }

public:
    // Constructor - Initializes the thread pool with specified number of worker threads
    // Parameter: num_threads - number of worker threads to create
    ThreadPool(size_t num_threads) : stop(false) {
        // Create the specified number of worker threads
        for (size_t i = 0; i < num_threads; ++i) {
            // Create a new thread and add it to workers vector
            // Each thread will execute the worker_loop function
            // Using member function pointer (no lambda)
            workers.emplace_back(&ThreadPool::worker_loop, this);
        }
    }

    // Template function to submit a task to the thread pool
    // Returns a future object that will contain the task's result
    // Parameters: f - function to execute, args - arguments to pass to the function
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        // Determine the return type of the function
        using return_type = typename std::result_of<F(Args...)>::type;
        
        // Create a packaged_task that wraps the function and its arguments
        // packaged_task allows us to get a future for the result
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        // Get the future object associated with this task
        // The future will contain the result once the task completes
        std::future<return_type> result = task->get_future();
        
        // Critical section - adding task to queue
        {
            // Acquire lock on the queue mutex
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // Check if thread pool has been stopped
            if (stop) {
                throw std::runtime_error("Cannot submit task to stopped ThreadPool");
            }
            
            // Add the task to the queue
            // Use std::bind to create a function wrapper (no lambda)
            // Binds the execute_packaged_task function with the task parameter
            tasks.emplace(std::bind(&ThreadPool::execute_packaged_task<return_type>, task));
            
            // Lock is automatically released when this block ends
        }
        
        // Notify one waiting worker thread that a task is available
        condition.notify_one();
        
        // Return the future object so caller can retrieve the result later
        return result;
    }

    // Function to get the current number of pending tasks in queue
    // Returns: number of tasks waiting to be executed
    size_t get_queue_size() {
        // Acquire lock to safely read queue size
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        // Return the number of tasks in queue
        return tasks.size();
    }

    // Function to gracefully shutdown the thread pool
    // Waits for all current tasks to complete before stopping
    void shutdown() {
        // Critical section - setting stop flag
        {
            // Acquire lock on the queue mutex
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // Set stop flag to true
            // This signals worker threads to exit after completing current tasks
            stop = true;
            
            // Lock is automatically released when this block ends
        }
        
        // Wake up all waiting worker threads
        // They will see the stop flag and exit
        condition.notify_all();
        
        // Wait for all worker threads to finish
        for (std::thread& worker : workers) {
            // Check if thread is joinable before joining
            if (worker.joinable()) {
                // Wait for this worker thread to complete
                worker.join();
            }
        }
    }

    // Destructor - automatically called when ThreadPool object is destroyed
    // Ensures proper cleanup of resources (RAII pattern)
    ~ThreadPool() {
        // Call shutdown to clean up all threads
        shutdown();
    }
};

#endif // THREADPOOL_H
