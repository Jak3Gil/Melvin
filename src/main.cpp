#include "core/AtomicGraph.h"
#include "core/BinaryPersistence.h"
#include "core/GraphStatistics.h"
#include "core/TaskQueue.h"
#include "intake/IntakeManager.h"
#include "intake/DatasetLoader.h"
#include "intake/MultimodalIntake.h"
#include "connections/ExactConnector.h"
#include "generalization/LeapNodes.h"
#include "generalization/LeapConnections.h"
#include "reasoning/ActivationField.h"
#include "reasoning/CoherenceCalculator.h"
#include "reasoning/TraversalEngine.h"
#include "motor/MotorController.h"
#include "motor/CANInterface.h"
#include "output/OutputManager.h"
#include "output/AudioOutput.h"
#include "output/VisualAttention.h"
#include "output/MotorOutput.h"
#include "output/TextOutput.h"
#include "output/Visualizer.h"
#include "evolution/EvolutionEngine.h"
#include "pruning/PruningEngine.h"
#include "feedback/FeedbackRouter.h"
#include "diagnostic/DebugTracer.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace melvin;

int main() {
    DebugTracer tracer;
    tracer.trace("=== MELVIN STARTING ===");
    
    std::cout << "Melvin v1.0 - Cognitive Architecture\n";
    std::cout << "Initializing systems...\n";
    
    // Initialize core graph
    auto graph = std::make_unique<AtomicGraph>();
    
    // Initialize statistics tracking
    auto stats = std::make_unique<GraphStatistics>();
    
    // Initialize parallel task queue (4 worker threads)
    auto task_queue = std::make_unique<TaskQueue>(4);
    
    // Initialize intake
    auto intake_manager = std::make_unique<IntakeManager>(graph.get());
    intake_manager->initialize_all();
    
    // Initialize dataset loader for text data
    auto dataset_loader = std::make_unique<DatasetLoader>(intake_manager.get(), graph.get());
    
    // Initialize CAN interface for motors
    auto can_interface = std::make_unique<CANInterface>("can0");
    can_interface->open(); // Will fail gracefully if hardware not present
    
    // Initialize motor control
    auto motor_controller = std::make_unique<MotorController>(graph.get(), can_interface.get());
    
    // Initialize outputs
    auto audio_output = std::make_unique<AudioOutput>();
    auto visual_attention = std::make_unique<VisualAttention>();
    auto motor_output = std::make_unique<MotorOutput>(motor_controller.get());
    auto text_output = std::make_unique<TextOutput>();
    
    auto output_manager = std::make_unique<OutputManager>(graph.get());
    output_manager->set_audio_output(audio_output.get());
    output_manager->set_visual_attention(visual_attention.get());
    output_manager->set_motor_output(motor_output.get());
    output_manager->set_text_output(text_output.get());
    
    // Initialize reasoning components
    auto activation_field = std::make_unique<ActivationField>(graph.get());
    auto coherence_calc = std::make_unique<CoherenceCalculator>(graph.get());
    auto traversal_engine = std::make_unique<TraversalEngine>(
        graph.get(), activation_field.get(), coherence_calc.get());
    
    // Initialize generalization
    auto leap_nodes = std::make_unique<LeapNodes>(graph.get());
    auto leap_connections = std::make_unique<LeapConnections>(graph.get());
    
    // Initialize evolution and pruning
    auto evolution_engine = std::make_unique<EvolutionEngine>();
    auto pruning_engine = std::make_unique<PruningEngine>(graph.get());
    
    // Initialize feedback
    auto feedback_router = std::make_unique<FeedbackRouter>(intake_manager.get(), output_manager.get());
    
    // Initialize connection formation
    auto exact_connector = std::make_unique<ExactConnector>(graph.get());
    
    // Initialize multimodal hardware intake
    auto multimodal_intake = std::make_unique<MultimodalIntake>(intake_manager.get(), graph.get());
    
    // Initialize visualizer for camera/audio display
    auto visualizer = std::make_unique<Visualizer>();
    
    std::cout << "Initialization complete.\n";
    
    // Initialize binary persistence
    auto persistence = std::make_unique<BinaryPersistence>(graph.get());
    
    // Try to load existing graph from binary files
    std::cout << "\nAttempting to load existing graph from binary files...\n";
    bool loaded = persistence->load_from_files("data/nodes.bin", "data/edges.bin");
    
    if (loaded) {
        std::cout << "Loaded " << persistence->get_nodes_count() << " nodes and " 
                  << persistence->get_edges_count() << " edges from binary files.\n";
    } else {
        std::cout << "No existing binary files found. Loading from text datasets...\n";
        
        // Load text data from files
        std::cout << "\nLoading dataset files...\n";
    }
    
    size_t nodes_loaded = 0;
    
    // Load various datasets
    nodes_loaded += dataset_loader->load_text_file("data/melvin_documentation.txt");
    std::cout << "Loaded " << nodes_loaded << " nodes from melvin_documentation.txt\n";
    
    nodes_loaded += dataset_loader->load_text_file("data/wikipedia_concepts.txt");
    std::cout << "Loaded additional nodes from wikipedia_concepts.txt\n";
    
    nodes_loaded += dataset_loader->load_text_file("data/literature.txt");
    std::cout << "Loaded additional nodes from literature.txt\n";
    
    nodes_loaded += dataset_loader->load_text_file("data/technical_docs.txt");
    std::cout << "Loaded additional nodes from technical_docs.txt\n";
    
    // Load simple test files
    dataset_loader->load_text_file("data/simple_test.txt");
    dataset_loader->load_text_file("data/test_input.txt");
    
    std::cout << "Total nodes in graph: " << graph->node_count() << "\n";
    std::cout << "Total edges in graph: " << graph->edge_count() << "\n";
    
    // Update statistics with loaded nodes
    for (size_t i = 0; i < graph->node_count(); ++i) {
        stats->node_created();
    }
    for (size_t i = 0; i < graph->edge_count(); ++i) {
        stats->edge_created();
    }
    
    tracer.trace("Starting main loop");
    std::cout << "\nRunning perception-action-reasoning loop...\n\n";
    
    // Start multimodal hardware intake (camera + microphone)
    std::cout << "Starting hardware capture (camera + microphone)...\n";
    // Note: Real hardware integration is now active!
    multimodal_intake->start();
    
    size_t cycle = 0;
    std::vector<NodeID> active_nodes;
    
    tracer.trace("Entering main loop");
    
    while (true) {
        cycle++;
        tracer.trace("Cycle " + std::to_string(cycle) + " started");
        
        // 1. INTAKE: Get sensory data
        // For demo, create some sample nodes
        if (cycle == 1) {
            // Create some test text input
            intake_manager->create_text_node('H');
            intake_manager->create_text_node('e');
            intake_manager->create_text_node('l');
            intake_manager->create_text_node('l');
            intake_manager->create_text_node('o');
            intake_manager->create_text_node(' ');
            intake_manager->create_text_node('M');
            intake_manager->create_text_node('e');
            intake_manager->create_text_node('l');
            intake_manager->create_text_node('v');
            intake_manager->create_text_node('i');
            intake_manager->create_text_node('n');
            intake_manager->create_text_node('!');
            intake_manager->create_text_node('\n');
        }
        
        // 2. CONNECTIONS: Form temporal connections (every cycle!)
        std::vector<NodeID> all_nodes = graph->get_all_nodes();
        
        // Connect the 3 most recent nodes to each other
        if (all_nodes.size() >= 3) {
            size_t n = all_nodes.size();
            // Connect last 3 nodes to each other
            for (size_t i = n - 3; i < n; ++i) {
                for (size_t j = i + 1; j < n; ++j) {
                    graph->add_edge(all_nodes[i], all_nodes[j], 1.0f);
                    graph->add_edge(all_nodes[j], all_nodes[i], 1.0f);
                }
            }
        }
        
        // Also use the connector for temporal connections
        if (all_nodes.size() > 1) {
            exact_connector->connect_temporal_neighbors(all_nodes.back());
        }
        
        // 3. GENERALIZATION: Create leap nodes
        if (cycle % 100 == 0) {
            auto leap_candidates = leap_nodes->find_candidates(evolution_engine->get_W());
            for (const auto& triple : leap_candidates) {
                if (triple.size() == 3) {
                    leap_nodes->consolidate_triple(triple[0], triple[1], triple[2]);
                }
            }
        }
        
        // 4. REASONING: Activate and traverse
        if (!active_nodes.empty()) {
            active_nodes = traversal_engine->reason(active_nodes);
        }
        
        // 5. OUTPUT: Route to appropriate channels
        output_manager->route_outputs(active_nodes);
        
        // Visualize camera and audio every 50 cycles
        if (cycle % 50 == 0 && cycle > 0) {
            std::vector<NodeID> all_nodes = graph->get_all_nodes();
            
            // Find and show recent vision node
            bool found_vision = false;
            for (int i = std::min(50, (int)all_nodes.size() - 1); i >= 0; --i) {
                Node* node = graph->get_node(all_nodes[i]);
                if (node && node->payload_size() == VISION_PAYLOAD_SIZE) {
                    visualizer->show_camera_frame((const uint8_t*)node->payload(), node->payload_size());
                    found_vision = true;
                    break;
                }
            }
            if (!found_vision) {
                std::cout << "\n(No vision nodes found yet)\n";
            }
            
            // Find and show recent audio node
            bool found_audio = false;
            for (int i = std::min(50, (int)all_nodes.size() - 1); i >= 0; --i) {
                Node* node = graph->get_node(all_nodes[i]);
                if (node && node->payload_size() == AUDIO_PAYLOAD_SIZE) {
                    visualizer->show_audio_waveform((const int16_t*)node->payload(), node->payload_size() / 2);
                    found_audio = true;
                    break;
                }
            }
            if (!found_audio) {
                std::cout << "\n┌─────────────────────────────────────────────────────────────┐\n";
                std::cout << "│ AUDIO WAVEFORM - No audio nodes captured yet                  │\n";
                std::cout << "│ Waiting for microphone input...                              │\n";
                std::cout << "└─────────────────────────────────────────────────────────────┘\n";
            }
            
            // Show stats
            visualizer->show_stats(graph->node_count(), graph->edge_count(), visual_attention->get_position());
        }
        
        // Sleep to prevent spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Slower for stability
        
        // Print every 20 cycles
        if (cycle % 20 == 0) {
            std::cout << "\n>>> Cycle " << cycle << " | Nodes: " << graph->node_count() << " | Edges: " << graph->edge_count() << "\n";
        }
        
        // 6. FEEDBACK: Feed outputs back to intake
        feedback_router->route_feedback(active_nodes);
        
        // 7. EVOLUTION: Update fitness and mutate parameters
        if (cycle % 100 == 0 && cycle > 0) {
            float coherence = coherence_calc->calculate_coherence(active_nodes);
            float accuracy = 0.5f; // TODO: calculate from prediction
            float efficiency = static_cast<float>(active_nodes.size()) / graph->node_count();
            
            evolution_engine->update(coherence, accuracy, efficiency);
        }
        
        // 8. PRUNING: Remove low-score nodes and edges
        if (cycle % 500 == 0) {
            size_t pruned_nodes = pruning_engine->prune_nodes();
            size_t pruned_edges = pruning_engine->prune_edges();
            
            if (pruned_nodes > 0 || pruned_edges > 0) {
                std::cout << "Pruned: " << pruned_nodes << " nodes, " << pruned_edges << " edges\n";
            }
        }
        
        // For demo purposes, exit after some cycles
        if (cycle > 1000) {
            break;
        }
    }
    
        std::cout << "\nShutting down...\n";
    
    // Stop hardware capture
    // multimodal_intake->stop();  // DISABLED
    
    // Print final statistics
    std::cout << "\n";
    stats->print_stats();
    
    // Save graph to binary files
    std::cout << "\nSaving graph to binary files...\n";
    bool saved = persistence->save_to_files("data/nodes.bin", "data/edges.bin");
    if (saved) {
        std::cout << "Saved " << graph->node_count() << " nodes and " 
                  << graph->edge_count() << " edges to data/nodes.bin and data/edges.bin\n";
    } else {
        std::cout << "Failed to save graph to binary files.\n";
    }
    
    // Cleanup
    intake_manager->shutdown_all();
    can_interface->close();
    
    return 0;
}

