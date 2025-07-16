#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

namespace Tokucu {

    class ThreadPool {
    public:
        ThreadPool(size_t threads);
        ~ThreadPool();

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
            -> std::future<typename std::invoke_result<F, Args...>::type>;

        void stop();

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop_threads = false;
    };

    inline ThreadPool::ThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop_threads || !this->tasks.empty(); });
                        if (this->stop_threads && this->tasks.empty()) {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if (stop_threads) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    inline void ThreadPool::stop() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop_threads = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

    inline ThreadPool::~ThreadPool() {
        if (!stop_threads) {
            stop();
        }
    }
} 