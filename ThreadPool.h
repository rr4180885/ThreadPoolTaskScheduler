#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
public:
    ThreadPool(size_t numThreads):  stop(false) {
        for(size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ThreadPool::workerLoop, this);
        }
    }
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            //critical section
            std::unique_lock<std::mutex> lock(mtx);
            if(stop) throw std::runtime_error("Submit on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        cv.notify_one(); // one worker wake up
        return res;
    }
    void shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for(std::thread &worker : workers) {
            if(worker.joinable())
                worker.join();
        }
    }
private:
    void workerLoop()
    {
        while(true)
        {
            std::function<void()> task;

            //critical section
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] {return stop || !tasks.empty();});
                if(stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            // end the critical section
            task(); // execute the task outside the critical section
        }
    }

};
