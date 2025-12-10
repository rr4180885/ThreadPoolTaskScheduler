#include "ThreadPool.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <future>
#include <iomanip>

std::string doTask(int taskId, int durationMs) {
    std::cout << "[Task " << taskId << "] Started, duration " << durationMs << "ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    std::stringstream ss;
    ss << "Task " << taskId << " finished after " << durationMs << "ms";
    std::cout << "[Task " << taskId << "] Finished\n";
    return ss.str();
}

int randomDuration(int minMs, int maxMs) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minMs, maxMs);
    return dist(gen);
}

int getUserInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < minVal || value > maxVal) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number between " << minVal << " and " << maxVal << "\n";
            continue;
        }
        break;
    }
    return value;
}

int main() {
    try {
        // Get number of threads and tasks from user
        int numThreads = getUserInput("Enter number of worker threads (1-16): ", 1, 16);
        int numTasks = getUserInput("Enter number of tasks (1-50): ", 1, 50);

        // Create thread pool
        ThreadPool pool(numThreads);
        std::vector<std::future<std::string>> results;

        // Submit tasks
        for (int i = 1; i <= numTasks; ++i) {
            int duration = randomDuration(500, 2000); // 0.5s to 2s
            results.push_back(pool.submit(doTask, i, duration));
        }

        // Collect results
        std::cout << "\n--- Task Results ---\n";
        for (size_t i = 0; i < results.size(); ++i) {
            std::string result = results[i].get();
            std::cout << result << "\n";
        }

        // Shutdown pool
        pool.shutdown();
        std::cout << "\nAll tasks completed. Thread pool shutdown.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
