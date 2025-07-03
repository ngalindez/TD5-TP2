cmake_minimum_required(VERSION 3.10)
project(CVRP_Solver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all source files
set(SRC_FILES
    src/Cliente.cpp
    src/Ruta.cpp
    src/Solucion.cpp
    src/CWwLocalSearch.cpp
    src/HeuristicaClarkeWright.cpp
    src/HeuristicaInsercionCercana.cpp
    src/OperadorRelocate.cpp
    src/OperadorSwap.cpp
)

# Gather all test files
file(GLOB TEST_FILES tests/test_*.cpp)



# Add the test executable (all tests + all sources)
add_executable(runTests ${TEST_FILES} ${SRC_FILES})

target_include_directories(runTests PRIVATE src tests)

# Enable testing and add a CTest entry
enable_testing()
add_test(NAME AllTests COMMAND runTests) 