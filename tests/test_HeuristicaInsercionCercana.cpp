#include "catch.hpp"
#include "../src/HeuristicaInsercionCercana.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

TEST_CASE("HeuristicaInsercionCercana: Construcción básica con datos válidos", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(1, 5), Cliente(2, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, 10, 1, 2));
}

TEST_CASE("HeuristicaInsercionCercana: Construcción con entrada vacía", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes;
    vector<vector<double>> distMatrix;
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, 10, 1, 1));
}

TEST_CASE("HeuristicaInsercionCercana: Construcción con entrada inválida (capacidad negativa)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(1, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, -1, 1, 1));
}

TEST_CASE("HeuristicaInsercionCercana: Inserción más cercana en instancia simple", "[HeuristicaInsercionCercana]") {
    // Para una instancia simple, debe visitar primero el cliente más cercano al depósito
    vector<Cliente> clientes = {Cliente(1, 5), Cliente(2, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    // Ambos clientes caben en un solo vehículo, se espera una sola ruta
    REQUIRE(sol.getRutas().size() == 1);
    // La ruta debe contener ambos clientes
    const auto& ruta = sol.getRutas()[0].getClientes();
    REQUIRE(std::find(ruta.begin(), ruta.end(), 1) != ruta.end());
    REQUIRE(std::find(ruta.begin(), ruta.end(), 2) != ruta.end());
}

TEST_CASE("HeuristicaInsercionCercana: Inicialización de rutas cuando no hay fusiones posibles", "[HeuristicaInsercionCercana]") {
    // Si la capacidad es muy pequeña, cada cliente debe estar en su propia ruta
    vector<Cliente> clientes = {Cliente(1, 8), Cliente(2, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaInsercionCercana: Todos los clientes en el depósito (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes; // Ningún cliente, solo depósito
    vector<vector<double>> distMatrix = {
        {0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    // No debe haber rutas con clientes
    REQUIRE(sol.getRutas().size() == 0);
}

TEST_CASE("HeuristicaInsercionCercana: Solo un cliente (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(1, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getRutas()[0].getClientes().size() == 2); // depósito + cliente
}

TEST_CASE("HeuristicaInsercionCercana: Todos los clientes caben en un vehículo (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(1, 3), Cliente(2, 4), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6, 7},
        {5, 0, 2, 3},
        {6, 2, 0, 4},
        {7, 3, 4, 0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    int totalDemanda = 0;
    for (const auto& c : clientes) totalDemanda += c.getDemand();
    REQUIRE(sol.getRutas()[0].getDemandaActual() == totalDemanda);
}

TEST_CASE("HeuristicaInsercionCercana: No hay fusiones factibles por capacidad (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(1, 8), Cliente(2, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaInsercionCercana: Regresión/Aleatorio solución siempre factible", "[HeuristicaInsercionCercana]") {
    std::mt19937 rng(42); // Semilla fija para reproducibilidad
    int n = 10;
    vector<Cliente> clientes;
    for (int i = 1; i <= n; ++i) {
        int demanda = 1 + (rng() % 5); // demanda 1-5
        clientes.push_back(Cliente(i, demanda));
    }
    vector<vector<double>> distMatrix(n+1, vector<double>(n+1, 0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i != j) distMatrix[i][j] = 1 + (rng() % 20);
        }
    }
    int capacidad = 10;
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, capacidad, 1, n);
    Solucion sol = heuristica.resolver();
    // Todas las rutas deben ser factibles
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= capacidad);
        REQUIRE(ruta.esFactible());
    }
}
