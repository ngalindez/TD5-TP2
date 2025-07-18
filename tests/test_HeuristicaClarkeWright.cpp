#include "catch.hpp"
#include "../src/HeuristicaClarkeWright.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

TEST_CASE("HeuristicaClarkeWright: Construcción básica con datos válidos", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 5), Cliente(2, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    REQUIRE_NOTHROW(HeuristicaClarkeWright(clientes, distMatrix, 10, 0, 2));
}

TEST_CASE("HeuristicaClarkeWright: Construcción con entrada vacía", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes;
    vector<vector<double>> distMatrix;
    REQUIRE_NOTHROW(HeuristicaClarkeWright(clientes, distMatrix, 10, 0, 1));
}

TEST_CASE("HeuristicaClarkeWright: Construcción con entrada inválida (capacidad negativa)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    REQUIRE_NOTHROW(HeuristicaClarkeWright(clientes, distMatrix, -1, 0, 1));
}

TEST_CASE("HeuristicaClarkeWright: Efecto de ahorros para instancia simple usando resolver", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 5), Cliente(2, 3)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getCostoTotal() == Approx(45.0));
}

TEST_CASE("HeuristicaClarkeWright: Efecto de inicialización de rutas (una por cliente si no hay fusiones posibles)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 8), Cliente(2, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaClarkeWright: Efecto de factibilidad y fusión (fusiona solo cuando es factible)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 6), Cliente(2, 6)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaClarkeWright: Todos los clientes en el depósito (caso borde)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0)};
    vector<vector<double>> distMatrix = {
        {0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 1);
    Solucion sol = heuristica.resolver();
    // No hay clientes, no hay rutas
    REQUIRE(sol.getRutas().size() == 0);
}

TEST_CASE("HeuristicaClarkeWright: Solo un cliente (caso borde)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    // La ruta debe tener depósito al inicio y al final
    REQUIRE(sol.getRutas()[0].getClientes().size() == 3);
}

TEST_CASE("HeuristicaClarkeWright: Todos los clientes caben en un vehículo (caso borde)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6, 7},
        {5, 0, 2, 3},
        {6, 2, 0, 4},
        {7, 3, 4, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 1);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 1);
    int totalDemanda = 0;
    for (const auto& c : clientes) totalDemanda += c.getId() == 0 ? 0 : c.getDemand();
    REQUIRE(sol.getRutas()[0].getDemandaActual() == totalDemanda);
}

TEST_CASE("HeuristicaClarkeWright: No hay fusiones factibles por capacidad (caso borde)", "[HeuristicaClarkeWright]") {
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 8), Cliente(2, 8)};
    vector<vector<double>> distMatrix = {
        {0, 10, 20},
        {10, 0, 15},
        {20, 15, 0}
    };
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion sol = heuristica.resolver();
    REQUIRE(sol.getRutas().size() == 2);
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= 10);
    }
}

TEST_CASE("HeuristicaClarkeWright: Regresión/Aleatorio solución siempre factible", "[HeuristicaClarkeWright]") {
    std::mt19937 rng(42);
    int n = 10;
    vector<Cliente> clientes;
    clientes.push_back(Cliente(0, 0));
    for (int i = 1; i <= n; ++i) {
        int demanda = 1 + (rng() % 5);
        clientes.push_back(Cliente(i, demanda));
    }
    vector<vector<double>> distMatrix(n+1, vector<double>(n+1, 0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i != j) distMatrix[i][j] = 1 + (rng() % 20);
        }
    }
    int capacidad = 10;
    HeuristicaClarkeWright heuristica(clientes, distMatrix, capacidad, 0, n);
    Solucion sol = heuristica.resolver();
    for (const auto& ruta : sol.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= capacidad);
        REQUIRE(ruta.esFactible());
    }
}
