#pragma once

namespace melvin {

class IntakeManager;

class TextIntake {
public:
    TextIntake(IntakeManager* manager);
    
    // Capture single character
    bool capture_character(char c);
    
    // Capture string (creates multiple nodes)
    void capture_string(const char* text);
    
private:
    IntakeManager* manager_;
};

} // namespace melvin

