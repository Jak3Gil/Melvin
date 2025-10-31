#include "CANInterface.h"
#include <cstring>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>
#endif

namespace melvin {

CANInterface::CANInterface(const std::string& interface) 
    : interface_name_(interface), socket_fd_(-1) {
}

CANInterface::~CANInterface() {
    close();
}

bool CANInterface::open() {
#ifdef __linux__
    // Create socket
    socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd_ < 0) {
        return false;
    }
    
    // Get interface index
    struct ifreq ifr;
    std::strcpy(ifr.ifr_name, interface_name_.c_str());
    if (ioctl(socket_fd_, SIOCGIFINDEX, &ifr) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    // Bind socket
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    
    if (bind(socket_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    
    return true;
#else
    // Stub for non-Linux systems
    return false;
#endif
}

void CANInterface::close() {
#ifdef __linux__
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
#endif
}

bool CANInterface::read_frame(uint16_t* id, uint8_t* data, size_t* len) {
#ifdef __linux__
    if (socket_fd_ < 0) {
        return false;
    }
    
    struct can_frame frame;
    ssize_t nbytes = ::read(socket_fd_, &frame, sizeof(frame));
    
    if (nbytes < 0) {
        return false;
    }
    
    *id = frame.can_id & 0x7FF; // 11-bit CAN ID
    *len = frame.can_dlc;
    std::memcpy(data, frame.data, *len);
    
    return true;
#else
    // Stub for non-Linux systems
    return false;
#endif
}

bool CANInterface::write_frame(uint16_t id, const uint8_t* data, size_t len) {
#ifdef __linux__
    if (socket_fd_ < 0 || len > 8) {
        return false;
    }
    
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = static_cast<uint8_t>(len);
    std::memcpy(frame.data, data, len);
    
    ssize_t nbytes = ::write(socket_fd_, &frame, sizeof(frame));
    return nbytes == sizeof(frame);
#else
    // Stub for non-Linux systems
    return false;
#endif
}

} // namespace melvin

