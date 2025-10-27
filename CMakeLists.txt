cmake_minimum_required(VERSION 3.15)
project(Melvin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

# Source files
set(CORE_SOURCES
    src/core/AtomicGraph.cpp
    src/core/Node.cpp
    src/core/NodeAllocator.cpp
    src/core/BinaryPersistence.cpp
    src/core/GraphStatistics.cpp
    src/core/TaskQueue.cpp
)

set(INTAKE_SOURCES
    src/intake/IntakeManager.cpp
    src/intake/VisionIntake.cpp
    src/intake/AudioIntake.cpp
    src/intake/TextIntake.cpp
    src/intake/MotorIntake.cpp
    src/intake/DatasetLoader.cpp
    src/intake/HardwareCapture.mm
    src/intake/MultimodalIntake.cpp
    src/intake/AdaptiveFilter.cpp
)

set(CONNECTION_SOURCES
    src/connections/ExactConnector.cpp
    src/connections/Weight.cpp
)

set(GENERALIZATION_SOURCES
    src/generalization/LeapNodes.cpp
    src/generalization/LeapConnections.cpp
)

set(REASONING_SOURCES
    src/reasoning/ActivationField.cpp
    src/reasoning/CoherenceCalculator.cpp
    src/reasoning/TraversalEngine.cpp
    src/reasoning/StabilityDetector.cpp
)

set(MOTOR_SOURCES
    src/motor/MotorNode.cpp
    src/motor/CANInterface.cpp
    src/motor/MotorController.cpp
)

set(OUTPUT_SOURCES
    src/output/OutputManager.cpp
    src/output/AudioOutput.cpp
    src/output/VisualAttention.cpp
    src/output/MotorOutput.cpp
    src/output/TextOutput.cpp
    src/output/Visualizer.cpp
)

set(EVOLUTION_SOURCES
    src/evolution/FitnessEvaluator.cpp
    src/evolution/ParameterMutator.cpp
    src/evolution/EvolutionEngine.cpp
)

set(PRUNING_SOURCES
    src/pruning/PruningEngine.cpp
)

set(DIAGNOSTIC_SOURCES
    src/diagnostic/DebugTracer.cpp
)

set(FEEDBACK_SOURCES
    src/feedback/FeedbackRouter.cpp
)

set(SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${INTAKE_SOURCES}
    ${CONNECTION_SOURCES}
    ${GENERALIZATION_SOURCES}
    ${REASONING_SOURCES}
    ${MOTOR_SOURCES}
    ${OUTPUT_SOURCES}
    ${EVOLUTION_SOURCES}
    ${PRUNING_SOURCES}
    ${FEEDBACK_SOURCES}
    ${DIAGNOSTIC_SOURCES}
)

# Build executable
# On macOS, enable Objective-C++ for hardware capture
if(APPLE)
    set_source_files_properties(src/intake/AVFoundationCapture.mm PROPERTIES
        COMPILE_FLAGS "-x objective-c++")
    
    add_executable(melvin ${SOURCES} src/intake/AVFoundationCapture.mm)
    
    # Link macOS frameworks for hardware capture
    target_link_libraries(melvin PRIVATE 
        pthread
        "-framework AVFoundation"
        "-framework CoreAudio"
        "-framework CoreVideo"
        "-framework CoreMedia"
        "-framework Foundation"
        "-framework AppKit"
    )
else()
    add_executable(melvin ${SOURCES})
    target_link_libraries(melvin PRIVATE pthread)
endif()

# On Linux, link socketcan for CAN bus
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(melvin PRIVATE rt)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(melvin PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(melvin PRIVATE DEBUG_BUILD)
endif()

