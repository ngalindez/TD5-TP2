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
    echo "Solucionador CVRP - Script de construcción y ejecución"
    echo ""
    echo "Uso: $0 [comando] [opciones]"
    echo ""
    echo "Comandos:"
    echo "  build                   Compilar el proyecto"
    echo "  clean                   Limpiar el directorio de compilación"
    echo "  test                    Ejecutar todos los tests"
    echo "  test-heuristica-CW      Ejecutar solo los tests de Clarke & Wright"
    echo "  test-heuristica-IC      Ejecutar solo los tests de Inserción Cercana"
    echo "  test-local              Ejecutar solo los tests de búsqueda local"
    echo "  test-operadores         Ejecutar solo los tests de operadores (swap y relocate)"
    echo "  run                     Ejecutar la aplicación principal"
    echo "  instancia <nombre|ruta> Ejecutar el solver en una instancia específica (nombre o ruta)"
    echo "  format                  Formatear el código fuente (si clang-format está disponible)"
    echo "  help                    Mostrar este mensaje de ayuda"
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
    local instance_arg="$1"
    local instance_file=""

    # If the argument is an existing file, use it directly
    if [ -f "$instance_arg" ]; then
        instance_file="$instance_arg"
    else
        # Try to build the path from the name
        instance_file="instancias/2l-cvrp-0/$instance_arg.dat"
        if [ ! -f "$instance_file" ]; then
            print_error "Archivo de instancia no encontrado: $instance_arg"
            echo "Instancias disponibles:"
            ls instancias/2l-cvrp-0/*.dat 2>/dev/null | sed 's/.*\///' | sed 's/\.dat$//' | head -10
            echo "... y más"
            exit 1
        fi
    fi

    if [ ! -f "$BIN_DIR/cvrp_solver" ]; then
        print_error "Ejecutable principal no encontrado. Por favor, compila el proyecto primero."
        exit 1
    fi

    print_info "Ejecutando solver en la instancia: $instance_file"
    "$BIN_DIR/cvrp_solver" "$instance_file"
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
    "test-heuristica-CW")
        run_tests "[HeuristicaClarkeWright]"
        ;;
    "test-heuristica-IC")
        run_tests "[HeuristicaInsercionCercana]"
        ;;
    "test-local")
        run_tests "[CWwLocalSearch]"
        ;;
    "test-operadores")
        run_tests "[OperadorSwap]"
        run_tests "[OperadorRelocate]"
        ;;
    "run")
        run_main
        ;;
    "instancia")
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