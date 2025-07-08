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

# Core library sources
set(CORE_SOURCES
    src/Cliente.cpp
    src/Ruta.cpp
    src/Solucion.cpp
    src/VRPLIBReader.cpp
)

# Algorithm sources
set(ALGORITHM_SOURCES
    src/HeuristicaClarkeWright.cpp
    src/HeuristicaInsercionCercana.cpp
    src/CWwLocalSearch.cpp
    src/OperadorSwap.cpp
    src/OperadorRelocate.cpp
    src/GRASP.cpp
)

# All sources combined
set(ALL_SOURCES ${CORE_SOURCES} ${ALGORITHM_SOURCES})

# Test files
file(GLOB TEST_FILES tests/test_*.cpp)

# =============================================================================
# Core library target
# =============================================================================

add_library(cvrp_core STATIC ${CORE_SOURCES})
target_include_directories(cvrp_core PUBLIC src)
target_compile_features(cvrp_core PUBLIC cxx_std_17)

# =============================================================================
# Algorithm library target
# =============================================================================

add_library(cvrp_algorithms STATIC ${ALGORITHM_SOURCES})
target_include_directories(cvrp_algorithms PUBLIC src)
target_compile_features(cvrp_algorithms PUBLIC cxx_std_17)
target_link_libraries(cvrp_algorithms cvrp_core)

# =============================================================================
# Main application target
# =============================================================================

add_executable(cvrp_solver src/main.cpp)
target_link_libraries(cvrp_solver cvrp_algorithms cvrp_core)
target_include_directories(cvrp_solver PRIVATE src)

# =============================================================================
# Main experiment target
# =============================================================================

add_executable(main_experiment experiments/main_experiment.cpp)
target_link_libraries(main_experiment cvrp_algorithms cvrp_core)
target_include_directories(main_experiment PRIVATE src)

# =============================================================================
# Test targets
# =============================================================================

# Main test executable
add_executable(runTests ${TEST_FILES})
target_link_libraries(runTests cvrp_algorithms cvrp_core)
target_include_directories(runTests PRIVATE src tests)

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
