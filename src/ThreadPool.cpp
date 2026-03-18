// ThreadPool.cpp
#include "ThreadPool.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <stdexcept>
ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
    for (std::thread& t : workers_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
}