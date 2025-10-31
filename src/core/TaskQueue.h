#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace melvin {

// Simple thread pool for parallel task execution
class TaskQueue {
public:
    TaskQueue(size_t num_threads = 4);
    ~TaskQueue();
    
    // Add task to queue
    void enqueue(std::function<void()> task);
    
    // Wait for all tasks to complete
    void wait_all();
    
    // Shutdown
    void shutdown();
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
    
    void worker_loop();
};

} // namespace melvin

