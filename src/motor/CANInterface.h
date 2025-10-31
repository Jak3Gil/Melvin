#pragma once

#include <cstdint>
#include <string>

namespace melvin {

// CAN bus interface (using SocketCAN on Linux)
class CANInterface {
public:
    CANInterface(const std::string& interface = "can0");
    ~CANInterface();
    
    // Open and close CAN socket
    bool open();
    void close();
    
    // Read CAN frame
    bool read_frame(uint16_t* id, uint8_t* data, size_t* len);
    
    // Write CAN frame
    bool write_frame(uint16_t id, const uint8_t* data, size_t len);
    
    // Check if interface is ready
    bool is_open() const { return socket_fd_ >= 0; }
    
private:
    std::string interface_name_;
    int socket_fd_;
};

} // namespace melvin

