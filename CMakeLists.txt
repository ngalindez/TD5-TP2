cmake_minimum_required(VERSION 3.10)
project(CVRP_Solver VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    # If Catch2 is not found, we'll use the header-only version
    message(STATUS "Catch2 not found, using header-only version")
endif()

# =============================================================================
# Source files organization
# =============================================================================

# List all your source files (except main.cpp and main_experiment.cpp)
set(SRC_FILES
    src/GRASP.cpp
    src/HeuristicaClarkeWright.cpp
    src/HeuristicaInsercionCercana.cpp
    src/OperadorRelocate.cpp
    src/OperadorSwap.cpp
    src/Ruta.cpp
    src/Solucion.cpp
    src/Cliente.cpp
    src/VRPLIBReader.cpp
    # ...add any other .cpp files needed
)

# Interactive menu executable
add_executable(solver src/main.cpp ${SRC_FILES})

# Experiment script-friendly executable
add_executable(experiment experiments/main_experiment.cpp ${SRC_FILES})

# If you need to link libraries (e.g., pthread, math), add:
# target_link_libraries(solver m)
# target_link_libraries(experiment m)

# =============================================================================
# Core library target
# =============================================================================

# =============================================================================
# Algorithm library target
# =============================================================================

# =============================================================================
# Main application target
# =============================================================================

# =============================================================================
# Test targets
# =============================================================================

# Main test executable
# Enable testing
enable_testing()

# Add CTest configuration
add_test(NAME AllTests COMMAND runTests)

# Individual test targets for convenience
add_custom_target(test-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target runTests
    COMMAND ${CMAKE_BINARY_DIR}/bin/runTests
    DEPENDS runTests
    COMMENT "Running all tests"
)

# Test targets for specific test suites
add_custom_target(test-heuristics
    COMMAND ${CMAKE_BINARY_DIR}/bin/runTests "[HeuristicaClarkeWright]"
    DEPENDS runTests
    COMMENT "Running heuristic tests"
)

add_custom_target(test-local-search
    COMMAND ${CMAKE_BINARY_DIR}/bin/runTests "[CWwLocalSearch]"
    DEPENDS runTests
    COMMENT "Running local search tests"
)

add_custom_target(test-insertion
    COMMAND ${CMAKE_BINARY_DIR}/bin/runTests "[HeuristicaInsercionCercana]"
    DEPENDS runTests
    COMMENT "Running insertion heuristic tests"
)

add_custom_target(test-operators
    COMMAND ${CMAKE_BINARY_DIR}/bin/runTests "[OperadorSwap]" "[OperadorRelocate]"
    DEPENDS runTests
    COMMENT "Running operator tests"
)

# =============================================================================
# Instance processing targets
# =============================================================================

# Function to create instance targets
function(add_instance_target instance_name)
    add_custom_target(instance-${instance_name}
        COMMAND ${CMAKE_BINARY_DIR}/bin/cvrp_solver
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS cvrp_solver
        COMMENT "Running solver on instance ${instance_name}"
    )
endfunction()

# Add targets for some example instances
add_instance_target(E022-04g)
add_instance_target(E051-05e)
add_instance_target(E076-07s)
add_instance_target(E101-08e)

# =============================================================================
# Utility targets
# =============================================================================

# Clean build
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Cleaning build directory"
)

# Format code (if clang-format is available)
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/tests/*.cpp
        COMMENT "Formatting source code"
    )
endif()

# =============================================================================
# Documentation
# =============================================================================

# Print configuration summary
message(STATUS "")
message(STATUS "=== CVRP Solver Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  cvrp_solver     - Main application")
message(STATUS "  runTests        - Test executable")
message(STATUS "  test-all        - Run all tests")
message(STATUS "  test-heuristics - Run heuristic tests")
message(STATUS "  test-local-search - Run local search tests")
message(STATUS "  test-insertion  - Run insertion heuristic tests")
message(STATUS "  test-operators  - Run operator tests")
message(STATUS "  instance-*      - Run solver on specific instances")
message(STATUS "  clean-build     - Clean build directory")
if(CLANG_FORMAT)
    message(STATUS "  format          - Format source code")
endif()
message(STATUS "")
message(STATUS "Usage examples:")
message(STATUS "  make test-all                    # Run all tests")
message(STATUS "  make test-heuristics             # Run heuristic tests only")
message(STATUS "  make instance-E022-04g           # Run solver on E022-04g instance")
message(STATUS "  ./bin/cvrp_solver                # Run main application")
message(STATUS "  ./bin/runTests                   # Run all tests directly")
message(STATUS "  ./bin/runTests \"[HeuristicaClarkeWright]\"  # Run specific test suite")
message(STATUS "") 