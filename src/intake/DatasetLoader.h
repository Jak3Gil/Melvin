#pragma once

#include "../core/AtomicGraph.h"
#include "IntakeManager.h"
#include <string>
#include <vector>

namespace melvin {

// Loads text datasets from files and feeds them into intake
class DatasetLoader {
public:
    DatasetLoader(IntakeManager* intake, AtomicGraph* graph);
    
    // Load text from file (one character per node)
    size_t load_text_file(const std::string& filename);
    
    // Load JSON lines format
    size_t load_jsonl(const std::string& filename, const std::string& text_field = "text");
    
private:
    IntakeManager* intake_;
    AtomicGraph* graph_;
    
    std::string read_file(const std::string& filename);
    std::vector<std::string> split_lines(const std::string& content);
};

} // namespace melvin

