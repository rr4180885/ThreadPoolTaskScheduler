// test_auto.cpp - Automated test version (no user input)
#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <random>
#include <iomanip>
#include <vector>

std::string execute_task(int task_id, double duration) {
    std::cout << "[Task " << task_id << "] Started - Duration: " 
              << std::fixed << std::setprecision(2) << duration << "s" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(duration * 1000)));
    std::stringstream result_stream;
    result_stream << "Task " << task_id << " completed successfully after " 
                  << std::fixed << std::setprecision(2) << duration << "s";
    std::string result = result_stream.str();
    std::cout << "[Task " << task_id << "] " << result << std::endl;
    return result;
}

double generate_random_duration(double min_duration, double max_duration) {
    static std::random_device random_device;
    static std::mt19937 generator(random_device());
    std::uniform_real_distribution<double> distribution(min_duration, max_duration);
    return distribution(generator);
}

int main() {
    try {
        int num_threads = 4;
        int num_tasks = 10;
        
        std::cout << "========================================" << std::endl;
        std::cout << "  AUTO TEST: " << num_threads << " threads, " << num_tasks << " tasks" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        ThreadPool pool(num_threads);
        std::vector<std::future<std::string>> futures;
        
        for (int i = 0; i < num_tasks; ++i) {
            int task_id = i + 1;
            double duration = generate_random_duration(0.5, 2.0);
            futures.push_back(pool.submit(execute_task, task_id, duration));
        }
        
        std::cout << "\nPending tasks: " << pool.get_queue_size() << "\n" << std::endl;
        
        for (size_t i = 0; i < futures.size(); ++i) {
            try {
                std::string result = futures[i].get();
                std::cout << "Result " << (i + 1) << ": " << result << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }
        
        pool.shutdown();
        std::cout << "\n✓ Test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
