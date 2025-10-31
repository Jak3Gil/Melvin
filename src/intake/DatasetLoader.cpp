#include "DatasetLoader.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace melvin {

DatasetLoader::DatasetLoader(IntakeManager* intake, AtomicGraph* graph)
    : intake_(intake), graph_(graph) {
}

size_t DatasetLoader::load_text_file(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        return 0;
    }
    
    std::string content = read_file(filename);
    
    if (content.empty()) {
        return 0;
    }
    
    size_t count = 0;
    for (char c : content) {
        intake_->create_text_node(c);
        count++;
    }
    
    return count;
}

std::string DatasetLoader::read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

size_t DatasetLoader::load_jsonl(const std::string& filename, const std::string& text_field) {
    // TODO: Implement JSONL parsing
    (void)filename;
    (void)text_field;
    return 0;
}

std::vector<std::string> DatasetLoader::split_lines(const std::string& content) {
    std::vector<std::string> lines;
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

} // namespace melvin

