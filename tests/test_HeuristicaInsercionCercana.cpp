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

TEST_CASE("Construcción básica con datos válidos", "[HeuristicaInsercionCercana]") {
    // IDs: 1 (depot), 2, 3  → need matrix 4×4 (0…3)
    vector<Cliente> clientes = { Cliente(2,5), Cliente(3,3) };
    vector<vector<double>> distMatrix = {
      { 0, 10, 20, 30 },
      { 10, 0, 15, 25 },
      { 20,15, 0, 35 },
      { 30,25,35, 0 }
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    REQUIRE_NOTHROW( HeuristicaInsercionCercana(clientes, distMatrix, id2pos, 10, 1, 2) );
}

TEST_CASE("Construcción con entrada vacía", "[HeuristicaInsercionCercana]") {
    // No clientes, depot=1 → matrix size ≥ 2
    vector<Cliente> clientes;
    vector<vector<double>> distMatrix = {
      {0, 1},
      {1, 0}
    };
    unordered_map<int,int> id2pos = {{1,0}};
    REQUIRE_NOTHROW( HeuristicaInsercionCercana(clientes, distMatrix, id2pos, 10, 1, 1) );
}

TEST_CASE("Construcción con capacidad negativa (no lanza)", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = { Cliente(2,5) };
    vector<vector<double>> distMatrix = {
      {0, 10},
      {10, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1}};
    REQUIRE_NOTHROW( HeuristicaInsercionCercana(clientes, distMatrix, id2pos, -5, 1, 1) );
}

TEST_CASE("Inserción sencilla en instancia mínima", "[HeuristicaInsercionCercana]") {
    // IDs: 1,2,3 → 4×4
    vector<Cliente> clientes = { Cliente(2,5), Cliente(3,3) };
    vector<vector<double>> distMatrix = {
      {0, 10, 20, 30},
      {10, 0, 15, 25},
      {20,15, 0, 35},
      {30,25,35, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = hi.resolver();

    REQUIRE( sol.getRutas().size() == 1 );
    const auto &ruta = sol.getRutas()[0].getClientes();
    // Debe contener ambos clientes 2 y 3
    REQUIRE( find(ruta.begin(), ruta.end(), 2) != ruta.end() );
    REQUIRE( find(ruta.begin(), ruta.end(), 3) != ruta.end() );
}

TEST_CASE("Dos clientes no fusionables → dos rutas separadas", "[HeuristicaInsercionCercana]") {
    // IDs: 1,2,3 → 4×4
    vector<Cliente> clientes = { Cliente(2,8), Cliente(3,8) };
    vector<vector<double>> distMatrix = {
      {0, 10, 20, 30},
      {10,0, 15, 25},
      {20,15,0, 35},
      {30,25,35, 0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = hi.resolver();

    REQUIRE( sol.getRutas().size() == 2 );
    for (const auto &r : sol.getRutas())
        REQUIRE( r.getDemandaActual() <= 10 );
}

TEST_CASE("Sin clientes → cero rutas", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes;
    // depot=1 → 2×2
    vector<vector<double>> distMatrix = {
      {0,1},
      {1,0}
    };
    unordered_map<int,int> id2pos={{1,0}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = hi.resolver();
    REQUIRE( sol.getRutas().empty() );
}

TEST_CASE("Un solo cliente → ruta única con 1 cliente", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = { Cliente(2,5) };
    // IDs 1,2 → matrix 3×3
    vector<vector<double>> distMatrix = {
      {0,10,20},
      {10,0,30},
      {20,30,0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = hi.resolver();

    REQUIRE( sol.getRutas().size() == 1 );
    REQUIRE( sol.getRutas()[0].getCantidadClientes() == 1 );
}

TEST_CASE("Todos caben en un solo vehículo", "[HeuristicaInsercionCercana]") {
    // IDs 1..4 → matrix 5×5
    vector<Cliente> clientes = {
      Cliente(2,3), Cliente(3,4), Cliente(4,2)
    };
    vector<vector<double>> distMatrix = {
      {0,5,6,7,8},
      {5,0,2,3,4},
      {6,2,0,4,5},
      {7,3,4,0,6},
      {8,4,5,6,0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2},{4,3}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 1);
    Solucion sol = hi.resolver();

    REQUIRE( sol.getRutas().size() == 1 );
    int suma = 0;
    for (auto &c: clientes) suma += c.getDemand();
    REQUIRE( sol.getRutas()[0].getDemandaActual() == suma );
}

TEST_CASE("Ni una fusión por capacidad → rutas separadas", "[HeuristicaInsercionCercana]") {
    vector<Cliente> clientes = { Cliente(2,8), Cliente(3,8) };
    vector<vector<double>> distMatrix = {
      {0,10,20,30},
      {10,0,15,25},
      {20,15,0,35},
      {30,25,35,0}
    };
    unordered_map<int,int> id2pos = {{1,0},{2,1},{3,2}};
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 1, 2);
    Solucion sol = hi.resolver();

    REQUIRE( sol.getRutas().size() == 2 );
    for (auto &r : sol.getRutas())
        REQUIRE( r.getDemandaActual() <= 10 );
}

TEST_CASE("Solución siempre factible (aleatorio)", "[HeuristicaInsercionCercana]") {
    mt19937 rng(42);
    int n = 10;
    vector<Cliente> clientes;
    unordered_map<int,int> id2pos;
    for (int i = 1; i <= n; ++i) {
        int d = 1 + (rng() % 5);
        clientes.emplace_back(i, d);
        id2pos[i] = i;
    }
    id2pos[0] = 0;  // depot=0
    vector<vector<double>> distMatrix(n+1, vector<double>(n+1, 0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i != j) distMatrix[i][j] = 1 + (rng() % 20);
        }
    }
    HeuristicaInsercionCercana hi(clientes, distMatrix, id2pos, 10, 0, n);
    Solucion sol = hi.resolver();

    for (const auto &r : sol.getRutas()) {
        REQUIRE( r.getDemandaActual() <= 10 );
        REQUIRE( r.esFactible() );
    }
}
