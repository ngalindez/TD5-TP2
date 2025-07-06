#!/bin/bash

# CVRP Solver - Convenient build and run script
# Usage: ./run.sh [command] [options]

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show help
show_help() {
    echo "CVRP Solver - Build and Run Script"
    echo ""
    echo "Usage: $0 [command] [options]"
    echo ""
    echo "Commands:"
    echo "  build           Build the project"
    echo "  clean           Clean build directory"
    echo "  test            Run all tests"
    echo "  test-heuristics Run heuristic tests only"
    echo "  test-local      Run local search tests only"
    echo "  test-insertion  Run insertion heuristic tests only"
    echo "  test-operators  Run operator tests only"
    echo "  run             Run the main application"
    echo "  instance <name> Run solver on specific instance"
    echo "  format          Format source code (if clang-format available)"
    echo "  help            Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 build                    # Build the project"
    echo "  $0 test                     # Run all tests"
    echo "  $0 test-heuristics          # Run heuristic tests"
    echo "  $0 run                      # Run main application"
    echo "  $0 instance E022-04g        # Run on E022-04g instance"
    echo ""
}

# Function to check if build directory exists
check_build() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_warning "Build directory not found. Creating it..."
        mkdir -p "$BUILD_DIR"
    fi
}

# Function to build the project
build_project() {
    print_info "Building CVRP Solver..."
    check_build
    
    cd "$BUILD_DIR"
    cmake ..
    make -j$(nproc)
    cd ..
    
    print_success "Build completed successfully!"
}

# Function to clean build
clean_build() {
    print_info "Cleaning build directory..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned!"
    else
        print_warning "Build directory does not exist."
    fi
}

# Function to run tests
run_tests() {
    local test_filter="$1"
    
    if [ ! -f "$BIN_DIR/runTests" ]; then
        print_error "Test executable not found. Please build the project first."
        exit 1
    fi
    
    if [ -n "$test_filter" ]; then
        print_info "Running tests with filter: $test_filter"
        "$BIN_DIR/runTests" "$test_filter"
    else
        print_info "Running all tests..."
        "$BIN_DIR/runTests"
    fi
}

# Function to run main application
run_main() {
    if [ ! -f "$BIN_DIR/cvrp_solver" ]; then
        print_error "Main executable not found. Please build the project first."
        exit 1
    fi
    
    print_info "Running CVRP Solver..."
    "$BIN_DIR/cvrp_solver"
}

# Function to run on specific instance
run_instance() {
    local instance_name="$1"
    
    if [ -z "$instance_name" ]; then
        print_error "Please specify an instance name."
        echo "Available instances:"
        ls instancias/2l-cvrp-0/*.dat | sed 's/.*\///' | sed 's/\.dat$//' | head -10
        echo "... and more"
        exit 1
    fi
    
    local instance_file="instancias/2l-cvrp-0/$instance_name.dat"
    if [ ! -f "$instance_file" ]; then
        print_error "Instance file not found: $instance_file"
        echo "Available instances:"
        ls instancias/2l-cvrp-0/*.dat | sed 's/.*\///' | sed 's/\.dat$//' | head -10
        echo "... and more"
        exit 1
    fi
    
    if [ ! -f "$BIN_DIR/cvrp_solver" ]; then
        print_error "Main executable not found. Please build the project first."
        exit 1
    fi
    
    print_info "Running solver on instance: $instance_name"
    echo "$instance_file" | "$BIN_DIR/cvrp_solver"
}

# Function to format code
format_code() {
    if ! command -v clang-format &> /dev/null; then
        print_error "clang-format not found. Please install it first."
        exit 1
    fi
    
    print_info "Formatting source code..."
    clang-format -i src/*.cpp src/*.h tests/*.cpp
    print_success "Code formatting completed!"
}

# Main script logic
case "${1:-help}" in
    "build")
        build_project
        ;;
    "clean")
        clean_build
        ;;
    "test")
        run_tests
        ;;
    "test-heuristics")
        run_tests "[HeuristicaClarkeWright]"
        ;;
    "test-local")
        run_tests "[CWwLocalSearch]"
        ;;
    "test-insertion")
        run_tests "[HeuristicaInsercionCercana]"
        ;;
    "test-operators")
        run_tests "[OperadorSwap]" "[OperadorRelocate]"
        ;;
    "run")
        run_main
        ;;
    "instance")
        run_instance "$2"
        ;;
    "format")
        format_code
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        print_error "Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac 