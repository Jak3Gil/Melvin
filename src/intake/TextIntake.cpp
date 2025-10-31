#include "TextIntake.h"
#include "IntakeManager.h"
#include <cstring>

namespace melvin {

TextIntake::TextIntake(IntakeManager* manager) : manager_(manager) {
}

bool TextIntake::capture_character(char c) {
    return manager_->create_text_node(c) != 0;
}

void TextIntake::capture_string(const char* text) {
    size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        capture_character(text[i]);
    }
}

} // namespace melvin

