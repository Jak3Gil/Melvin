#include "BinaryPersistence.h"
#include "Node.h"
#include <fstream>
#include <cstring>

namespace melvin {

BinaryPersistence::BinaryPersistence(AtomicGraph* graph) : graph_(graph) {
}

bool BinaryPersistence::save_to_files(const std::string& nodes_file, const std::string& edges_file) {
    // Save nodes
    {
        std::ofstream file(nodes_file, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::vector<NodeID> all_nodes = graph_->get_all_nodes();
        nodes_count_ = all_nodes.size();
        
        for (NodeID node_id : all_nodes) {
            Node* node = graph_->get_node(node_id);
            if (!node) continue;
            
            NodeRecord record;
            record.id = node->id();
            record.frequency = node->frequency();
            record.first_seen = node->first_seen();
            record.payload_size = node->payload_size();
            
            // Write node record
            file.write(reinterpret_cast<const char*>(&record), sizeof(NodeRecord));
            
            // Write payload
            if (record.payload_size > 0) {
                file.write(reinterpret_cast<const char*>(node->payload()), record.payload_size);
            }
        }
    }
    
    // Save edges
    {
        std::ofstream file(edges_file, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        std::vector<Edge> all_edges = graph_->get_all_edges();
        edges_count_ = all_edges.size();
        
        for (const Edge& edge : all_edges) {
            EdgeRecord record;
            record.source = edge.source;
            record.target = edge.target;
            record.weight = edge.weight;
            
            file.write(reinterpret_cast<const char*>(&record), sizeof(EdgeRecord));
        }
    }
    
    return true;
}

bool BinaryPersistence::load_from_files(const std::string& nodes_file, const std::string& edges_file) {
    nodes_count_ = 0;
    edges_count_ = 0;
    
    // Load nodes
    {
        std::ifstream file(nodes_file, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        while (file.good()) {
            NodeRecord record;
            file.read(reinterpret_cast<char*>(&record), sizeof(NodeRecord));
            
            if (file.gcount() != sizeof(NodeRecord)) {
                break;
            }
            
            // Read payload
            std::vector<uint8_t> payload(record.payload_size);
            if (record.payload_size > 0) {
                file.read(reinterpret_cast<char*>(payload.data()), record.payload_size);
            }
            
            // Create node
            auto node = std::make_unique<Node>(record.id, payload.data(), record.payload_size);
            node->set_frequency(record.frequency);
            node->set_first_seen(record.first_seen);
            
            graph_->add_node(std::move(node));
            nodes_count_++;
        }
    }
    
    // Load edges
    {
        std::ifstream file(edges_file, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        while (file.good()) {
            EdgeRecord record;
            file.read(reinterpret_cast<char*>(&record), sizeof(EdgeRecord));
            
            if (file.gcount() != sizeof(EdgeRecord)) {
                break;
            }
            
            graph_->add_edge(record.source, record.target, record.weight);
            edges_count_++;
        }
    }
    
    return true;
}

} // namespace melvin

