#include "DisplayManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace melvin {

DisplayManager::DisplayManager() : active_(false), show_overlay_(true) {
#ifdef HAVE_OPENCV
    std::cout << "[Display] Initializing OpenCV display manager...\n";
#endif
}

DisplayManager::~DisplayManager() {
    shutdown();
}

bool DisplayManager::init() {
#ifdef HAVE_OPENCV
    try {
        create_camera_window();
        create_audio_window();
        active_ = true;
        std::cout << "[Display] OpenCV windows created successfully\n";
        std::cout << "[Display] Press ESC to quit, 's' to toggle overlay\n";
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "[Display] OpenCV error: " << e.what() << "\n";
        return false;
    }
#else
    std::cout << "[Display] OpenCV not available, using ASCII fallback\n";
    active_ = true;
    return true;
#endif
}

void DisplayManager::shutdown() {
    if (!active_.load()) return;
    
    active_.store(false);
    
#ifdef HAVE_OPENCV
    cv::destroyAllWindows();
    std::cout << "[Display] Closed all windows\n";
#endif
}

void DisplayManager::update_camera_frame(const uint8_t* frame_data, size_t frame_size,
                                         const GraphStats& stats, bool show_attention) {
    if (!frame_data || frame_size < 768) return;
    
#ifdef HAVE_OPENCV
    try {
        std::lock_guard<std::mutex> lock(frame_mutex_);
        
        // Upscale 16x16 to 256x256
        upscale_frame(frame_data, camera_frame_);
        
        // Draw overlay if enabled
        if (show_overlay_ && show_attention) {
            draw_camera_overlay(stats);
        }
        
        // Display the frame
        imshow("Melvin Camera", camera_frame_);
    } catch (const cv::Exception& e) {
        std::cerr << "[Display] Camera update error: " << e.what() << "\n";
    }
#else
    print_ascii_camera(frame_data, stats);
#endif
}

void DisplayManager::update_audio_waveform(const int16_t* audio_data, size_t audio_size) {
    if (!audio_data || audio_size == 0) return;
    
#ifdef HAVE_OPENCV
    try {
        std::lock_guard<std::mutex> lock(frame_mutex_);
        
        draw_audio_waveform(audio_data, audio_size);
        
        imshow("Melvin Audio", audio_frame_);
    } catch (const cv::Exception& e) {
        std::cerr << "[Display] Audio update error: " << e.what() << "\n";
    }
#else
    print_ascii_audio(audio_data, audio_size);
#endif
}

bool DisplayManager::process_events() {
#ifdef HAVE_OPENCV
    int key = cv::waitKey(1) & 0xFF;
    
    if (key == 27) { // ESC
        std::cout << "[Display] ESC pressed, shutting down...\n";
        shutdown();
        return false;
    } else if (key == 's' || key == 'S') {
        show_overlay_ = !show_overlay_;
        std::cout << "[Display] Overlay " << (show_overlay_ ? "enabled" : "disabled") << "\n";
    }
#endif
    
    return true;
}

#ifdef HAVE_OPENCV

void DisplayManager::create_camera_window() {
    namedWindow("Melvin Camera", cv::WINDOW_AUTOSIZE);
    cv::resizeWindow("Melvin Camera", 512, 512);
    
    // Initialize frame buffer (256x256 for upscaled 16x16)
    camera_frame_ = cv::Mat::zeros(256, 256, CV_8UC3);
}

void DisplayManager::create_audio_window() {
    namedWindow("Melvin Audio", cv::WINDOW_AUTOSIZE);
    cv::resizeWindow("Melvin Audio", 800, 200);
    
    // Initialize audio frame buffer
    audio_frame_ = cv::Mat::zeros(200, 800, CV_8UC3);
}

void DisplayManager::upscale_frame(const uint8_t* src, cv::Mat& dst) {
    // 16x16 RGB input -> 256x256 RGB output (16x upscale)
    for (int y = 0; y < 256; ++y) {
        for (int x = 0; x < 256; ++x) {
            int src_x = x / 16;
            int src_y = y / 16;
            
            int src_idx = (src_y * 16 + src_x) * 3;
            
            if (src_idx < 768) {
                cv::Vec3b pixel;
                pixel[0] = src[src_idx + 0];     // B
                pixel[1] = src[src_idx + 1];     // G
                pixel[2] = src[src_idx + 2];     // R
                
                dst.at<cv::Vec3b>(y, x) = pixel;
            }
        }
    }
}

void DisplayManager::draw_camera_overlay(const GraphStats& stats) {
    // Draw attention box (red rectangle)
    cv::Rect attention_box(
        static_cast<int>(stats.attention_x * 256 / 16),
        static_cast<int>(stats.attention_y * 256 / 16),
        16 * 16,  // Attention box is 1x1 in 16x16, scaled to 16x16 in 256x256
        16 * 16
    );
    cv::rectangle(camera_frame_, attention_box, cv::Scalar(0, 0, 255), 2);
    
    // Draw "ATTENTION" label
    cv::putText(camera_frame_, "ATTENTION", 
                cv::Point(attention_box.x, attention_box.y - 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
    
    // Draw stats text
    std::stringstream ss;
    ss << "Nodes: " << stats.nodes << "  Edges: " << stats.edges;
    cv::putText(camera_frame_, ss.str(),
                cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    
    // Draw weight
    std::stringstream ss2;
    ss2 << "Weight: " << std::fixed << std::setprecision(2) << stats.attention_weight;
    cv::putText(camera_frame_, ss2.str(),
                cv::Point(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

void DisplayManager::draw_audio_waveform(const int16_t* audio_data, size_t audio_size) {
    // Clear frame
    audio_frame_ = cv::Scalar(0, 0, 0);
    
    // Center line
    cv::line(audio_frame_, cv::Point(0, 100), cv::Point(800, 100),
              cv::Scalar(128, 128, 128), 1);
    
    if (!audio_data || audio_size == 0) return;
    
    // Draw waveform (green line)
    const int width = 800;
    const int height = 200;
    const int center_y = 100;
    const float scale = 80.0f / 32767.0f; // Scale to fit in ±80 pixels
    
    for (size_t i = 0; i + 1 < audio_size && i + 1 < width; ++i) {
        float y1 = static_cast<float>(audio_data[i]) * scale;
        float y2 = static_cast<float>(audio_data[i + 1]) * scale;
        
        int x1 = static_cast<int>(i);
        int x2 = static_cast<int>(i + 1);
        int py1 = center_y - static_cast<int>(y1);
        int py2 = center_y - static_cast<int>(y2);
        
        cv::line(audio_frame_, 
                 cv::Point(x1, std::max(0, std::min(height-1, py1))),
                 cv::Point(x2, std::max(0, std::min(height-1, py2))),
                 cv::Scalar(0, 255, 0), 1);
    }
    
    // Draw sample info
    std::stringstream ss;
    ss << "Samples: " << audio_size << " @ 20ms";
    cv::putText(audio_frame_, ss.str(),
                cv::Point(10, 180),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

#else

void DisplayManager::print_ascii_camera(const uint8_t* frame_data, const GraphStats& stats) {
    std::cout << "\033[2J\033[H"; // Clear screen
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ MELVIN CAMERA VIEW (16×16)                        Nodes: " 
              << std::setw(6) << stats.nodes << " │\n";
    std::cout << "│                                                  Edges: " 
              << std::setw(6) << stats.edges << " │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
    
    for (size_t y = 0; y < 16; ++y) {
        std::cout << "│ ";
        for (size_t x = 0; x < 16; ++x) {
            size_t idx = (y * 16 + x) * 3;
            uint8_t r = frame_data[idx];
            uint8_t g = frame_data[idx + 1];
            uint8_t b = frame_data[idx + 2];
            
            // Convert to grayscale
            uint8_t gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
            
            // Simple ASCII art
            char ch = ' ';
            if (gray > 200) ch = '##';
            else if (gray > 150) ch = '##';
            else if (gray > 100) ch = '##';
            else if (gray > 50) ch = '::';
            else if (gray > 25) ch = '..';
            
            std::cout << ch;
        }
        std::cout << " │\n";
    }
}

void DisplayManager::print_ascii_audio(const int16_t* audio_data, size_t audio_size) {
    std::cout << "\n┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ AUDIO WAVEFORM (20ms @ 16kHz)                              │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
    
    const int width = 80;
    std::vector<int> levels(width, 0);
    
    for (size_t i = 0; i < audio_size && i < 320; ++i) {
        int16_t sample = audio_data[i];
        size_t bin = (i * width) / audio_size;
        if (bin < width) {
            int level = std::abs(sample) / 1000;
            if (level > levels[bin]) {
                levels[bin] = level;
            }
        }
    }
    
    std::cout << "│";
    for (auto level : levels) {
        std::cout << (level > 20 ? "#" : level > 10 ? ":" : level > 5 ? "." : " ");
    }
    std::cout << "│\n";
}

#endif

} // namespace melvin

