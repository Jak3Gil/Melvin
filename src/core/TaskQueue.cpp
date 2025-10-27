#include "TaskQueue.h"

namespace melvin {

TaskQueue::TaskQueue(size_t num_threads) : stop_(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&TaskQueue::worker_loop, this);
    }
}

TaskQueue::~TaskQueue() {
    shutdown();
}

void TaskQueue::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
    }
    condition_.notify_one();
}

void TaskQueue::wait_all() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return tasks_.empty(); });
}

void TaskQueue::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void TaskQueue::worker_loop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { return !tasks_.empty() || stop_; });
            
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        
        task();
        
        // Notify that a task completed
        condition_.notify_one();
    }
}

} // namespace melvin

