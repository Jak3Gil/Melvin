#pragma once

#include <iostream>
#include <chrono>
#include <string>

namespace melvin {

// Tracks execution time for each operation to find bottlenecks
class DebugTracer {
public:
    DebugTracer() : start_time_(std::chrono::steady_clock::now()) {}
    
    void trace(const std::string& operation) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time_).count();
        
        std::cout << "[" << elapsed << "ms] " << operation << std::endl;
        
        last_operation_ = operation;
        last_time_ = now;
    }
    
    void checkpoint() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_time_).count();
        
        if (elapsed > 10) {
            std::cout << "  ⚠️  " << last_operation_ << " took " << elapsed << "ms" << std::endl;
        }
    }
    
private:
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_time_;
    std::string last_operation_ = "start";
};

} // namespace melvin

