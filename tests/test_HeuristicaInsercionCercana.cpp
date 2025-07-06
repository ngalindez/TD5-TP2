#include "catch.hpp"
#include "../src/HeuristicaInsercionCercana.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

using namespace std;

TEST_CASE("HeuristicaInsercionCercana: Construcción básica con datos válidos", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, id2pos, 10, 1, 2));
}

TEST_CASE("HeuristicaInsercionCercana: Construcción con entrada vacía", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes;
    vector<vector<double>> distMatrix;
    unordered_map<int,int> id2pos = {{1,0}};
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, id2pos, 10, 1, 1));
}

TEST_CASE("HeuristicaInsercionCercana: Construcción con entrada inválida (capacidad negativa)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1}};
    REQUIRE_NOTHROW(HeuristicaInsercionCercana(clientes, distMatrix, id2pos, -1, 1, 1));
}

TEST_CASE("HeuristicaInsercionCercana: Inserción más cercana en instancia simple", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    const auto& ruta = sol.getRutas()[0].getClientes();
    REQUIRE(std::find(ruta.begin(), ruta.end(), 2) != ruta.end());
    REQUIRE(std::find(ruta.begin(), ruta.end(), 3) != ruta.end());
}

TEST_CASE("HeuristicaInsercionCercana: Inicialización de rutas cuando no hay fusiones posibles", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 8), Cliente(3, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaInsercionCercana: Todos los clientes en el depósito (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes; // Ningún cliente
    vector<vector<double>> distMatrix = {
        {0}
    };
    unordered_map<int,int> id2pos = {{1,0}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 0);
}

TEST_CASE("HeuristicaInsercionCercana: Solo un cliente (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getRutas()[0].getCantidadClientes() == 1); // solo el cliente (sin depósitos)
}

TEST_CASE("HeuristicaInsercionCercana: Todos los clientes caben en un vehículo (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 3), Cliente(3, 4), Cliente(4, 2)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6, 7},
        {5, 0, 2, 3},
        {6, 2, 0, 4},
        {7, 3, 4, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2},{4,3}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    int totalDemanda = 0;
    for (const auto& c : clientes) totalDemanda += c.getDemand();
    REQUIRE(sol.getRutas()[0].getDemandaActual() == totalDemanda);
}

TEST_CASE("HeuristicaInsercionCercana: No hay fusiones factibles por capacidad (caso borde)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = {Cliente(2, 8), Cliente(3, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaInsercionCercana: Regresión/Aleatorio solución siempre factible", "[HeuristicaInsercionCercana]") {
    std::mt19937 rng(42);
    int n = 10;
    vector<Cliente> clientes;
    unordered_map<int,int> id2pos;
    for (int i=1; i<=n; ++i) {
        int demanda = 1 + (rng() % 5);
        clientes.push_back(Cliente(i, demanda));
        id2pos[i] = i;
    }
    id2pos[0]=0;
    vector<vector<double>> distMatrix(n+1, vector<double>(n+1,0));
    for (int i=0; i<=n; ++i) {
        for (int j=0; j<=n; ++j) {
            if (i!=j) distMatrix[i][j] = 1+(rng()%20);
        }
    }
    int capacidad=10;
    HeuristicaInsercionCercana heuristica(clientes, distMatrix, id2pos, capacidad, 0, n);
    Solucion sol=heuristica.resolver();
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= capacidad);
        REQUIRE(ruta.esFactible());
    }
}
