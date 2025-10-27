// Quick test to verify binary persistence works

#include <iostream>
#include <fstream>
#include <cstdint>

int main() {
    std::cout << "Testing binary file format...\n\n";
    
    // Create test data
    struct TestNode {
        uint64_t id;
        uint32_t frequency;
        uint64_t first_seen;
        size_t payload_size;
    };
    
    struct TestEdge {
        uint64_t source;
        uint64_t target;
        uint16_t weight;
    };
    
    // Test node
    TestNode node = {1, 5, 1000, 1};
    char payload = 'A';
    
    std::cout << "Node structure size: " << sizeof(TestNode) << " bytes\n";
    std::cout << "Edge structure size: " << sizeof(TestEdge) << " bytes\n\n";
    
    // Write test
    {
        std::ofstream file("data/test_nodes.bin", std::ios::binary);
        file.write(reinterpret_cast<const char*>(&node), sizeof(TestNode));
        file.write(&payload, 1);
        std::cout << "Wrote node to test_nodes.bin\n";
    }
    
    // Read test
    {
        std::ifstream file("data/test_nodes.bin", std::ios::binary);
        TestNode read_node;
        file.read(reinterpret_cast<char*>(&read_node), sizeof(TestNode));
        char read_payload;
        file.read(&read_payload, 1);
        
        std::cout << "Read back: ID=" << read_node.id 
                  << ", freq=" << read_node.frequency
                  << ", payload=" << read_payload << "\n";
    }
    
    return 0;
}

