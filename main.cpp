// main.cpp
// Thread Pool Task Scheduler - Main Program
// This program demonstrates concurrent task execution using a thread pool
// Author: [Your Name]
// Features: Dynamic thread count, random task generation, user input

#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <random>
#include <iomanip>
#include <vector>
#include <limits>

// Function to simulate a task with specific ID and duration
// This function will be executed by worker threads in the pool
// Parameters:
//   task_id - unique identifier for this task
//   duration - how long the task should run (in seconds)
// Returns: a string containing the task completion message
std::string execute_task(int task_id, double duration) {
    // Print task start message with ID and duration
    std::cout << "[Task " << task_id << "] Started - Duration: " 
              << std::fixed << std::setprecision(2) << duration << "s" << std::endl;
    
    // Convert duration from seconds to milliseconds for sleep function
    int milliseconds = static_cast<int>(duration * 1000);
    
    // Simulate work by putting thread to sleep for specified duration
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    
    // Create a string stream to build the result message
    std::stringstream result_stream;
    
    // Build the completion message with task ID and duration
    result_stream << "Task " << task_id << " completed successfully after " 
                  << std::fixed << std::setprecision(2) << duration << "s";
    
    // Convert string stream to string
    std::string result = result_stream.str();
    
    // Print task completion message
    std::cout << "[Task " << task_id << "] " << result << std::endl;
    
    // Return the result string
    return result;
}

// Function to generate a random duration between min and max seconds
// Parameters:
//   min_duration - minimum duration in seconds
//   max_duration - maximum duration in seconds
// Returns: a random duration value between min and max
double generate_random_duration(double min_duration, double max_duration) {
    // Create a random device to seed the random number generator
    static std::random_device random_device;
    
    // Create a Mersenne Twister random number generator
    static std::mt19937 generator(random_device());
    
    // Create a uniform distribution between min and max duration
    std::uniform_real_distribution<double> distribution(min_duration, max_duration);
    
    // Generate and return a random duration
    return distribution(generator);
}

// Function to get valid integer input from user
// Parameters:
//   prompt - message to display to user
//   min_value - minimum acceptable value
//   max_value - maximum acceptable value
// Returns: valid integer within specified range
int get_user_input(const std::string& prompt, int min_value, int max_value) {
    // Variable to store user input
    int value;
    
    // Loop until user provides valid input
    while (true) {
        // Display prompt to user
        std::cout << prompt;
        
        // Read input from user
        std::cin >> value;
        
        // Check if input is valid (not a character or invalid type)
        if (std::cin.fail()) {
            // Clear error flags on cin
            std::cin.clear();
            
            // Discard invalid input from input buffer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            // Inform user of invalid input
            std::cout << "Invalid input! Please enter a number." << std::endl;
            
            // Continue to next iteration to ask again
            continue;
        }
        
        // Check if value is within acceptable range
        if (value < min_value || value > max_value) {
            // Inform user that value is out of range
            std::cout << "Please enter a value between " << min_value 
                      << " and " << max_value << std::endl;
            
            // Continue to next iteration to ask again
            continue;
        }
        
        // Input is valid, break out of loop
        break;
    }
    
    // Return the valid value
    return value;
}

// Function to display program header and information
void display_header() {
    // Display program title with decorative border
    std::cout << "========================================" << std::endl;
    std::cout << "  THREAD POOL TASK SCHEDULER" << std::endl;
    std::cout << "  Dynamic Concurrent Task Execution" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

// Function to display task execution summary
// Parameters:
//   num_threads - number of worker threads used
//   num_tasks - number of tasks executed
void display_summary(int num_threads, int num_tasks) {
    // Display summary header
    std::cout << "\n========================================" << std::endl;
    std::cout << "  EXECUTION SUMMARY" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Display number of threads used
    std::cout << "Worker Threads: " << num_threads << std::endl;
    
    // Display number of tasks executed
    std::cout << "Tasks Executed: " << num_tasks << std::endl;
    
    // Display completion message
    std::cout << "Status: All tasks completed successfully" << std::endl;
    std::cout << "========================================" << std::endl;
}

// Main function - Entry point of the program
int main() {
    // Wrap entire program in try-catch block for exception handling
    try {
        // Display program header
        display_header();
        
        // Get number of worker threads from user
        // Minimum: 1 thread, Maximum: 16 threads (reasonable limit)
        int num_threads = get_user_input(
            "Enter number of worker threads (1-16): ", 
            1, 
            16
        );
        
        // Get number of tasks to execute from user
        // Minimum: 1 task, Maximum: 100 tasks (reasonable limit)
        int num_tasks = get_user_input(
            "Enter number of tasks to execute (1-100): ", 
            1, 
            100
        );
        
        // Display blank line for readability
        std::cout << std::endl;
        
        // Inform user that thread pool is being created
        std::cout << "Creating thread pool with " << num_threads 
                  << " worker threads..." << std::endl;
        
        // Create ThreadPool object with user-specified number of threads
        ThreadPool pool(num_threads);
        
        // Display success message
        std::cout << "Thread pool created successfully!" << std::endl;
        std::cout << std::endl;
        
        // Create vector to store future objects for all tasks
        // Futures allow us to retrieve results when tasks complete
        std::vector<std::future<std::string>> futures;
        
        // Reserve space in vector to avoid reallocations
        futures.reserve(num_tasks);
        
        // Inform user that tasks are being submitted
        std::cout << "Submitting " << num_tasks << " tasks to the thread pool..." 
                  << std::endl;
        std::cout << std::endl;
        
        // Loop to submit all tasks to the thread pool
        for (int i = 0; i < num_tasks; ++i) {
            // Calculate task ID (starting from 1 for readability)
            int task_id = i + 1;
            
            // Generate random duration between 0.5 and 3.0 seconds
            double duration = generate_random_duration(0.5, 3.0);
            
            // Submit task to thread pool and store the returned future
            // The task will execute the execute_task function with task_id and duration
            futures.push_back(pool.submit(execute_task, task_id, duration));
        }
        
        // Get current number of pending tasks in queue
        size_t pending_tasks = pool.get_queue_size();
        
        // Display queue status
        std::cout << "\nTasks submitted! Pending in queue: " << pending_tasks << std::endl;
        std::cout << "\n--- Tasks Executing Concurrently ---\n" << std::endl;
        
        // Wait for all tasks to complete and collect results
        std::cout << "\n--- Collecting Task Results ---\n" << std::endl;
        
        // Counter for successfully completed tasks
        int completed_tasks = 0;
        
        // Loop through all futures to retrieve results
        for (size_t i = 0; i < futures.size(); ++i) {
            // Try to get result from future
            try {
                // Call get() on future - this blocks until task completes
                // Returns the string result from execute_task function
                std::string result = futures[i].get();
                
                // Increment completed tasks counter
                completed_tasks++;
                
                // Display result number and message
                std::cout << "Result " << (i + 1) << ": " << result << std::endl;
                
            } catch (const std::exception& e) {
                // If task threw an exception, catch it here
                // Display error message with task number
                std::cerr << "Result " << (i + 1) << " - Exception: " 
                          << e.what() << std::endl;
            }
        }
        
        // Display blank line for readability
        std::cout << std::endl;
        
        // Inform user that thread pool is shutting down
        std::cout << "Initiating thread pool shutdown..." << std::endl;
        
        // Gracefully shutdown the thread pool
        // This waits for all threads to finish their current tasks
        pool.shutdown();
        
        // Display shutdown confirmation
        std::cout << "Thread pool shutdown complete." << std::endl;
        
        // Display execution summary
        display_summary(num_threads, completed_tasks);
        
    } catch (const std::exception& e) {
        // Catch any exceptions that weren't handled elsewhere
        // Display error message
        std::cerr << "\nError occurred: " << e.what() << std::endl;
        
        // Return error code 1 to indicate program failure
        return 1;
    }
    
    // Return 0 to indicate successful program completion
    return 0;
}
