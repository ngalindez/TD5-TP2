#include "catch.hpp"
#include "../src/CWwLocalSearch.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

TEST_CASE("CWwLocalSearch: Construcción básica con datos válidos", "[CWwLocalSearch]") {
    // Prueba de construcción básica
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    REQUIRE_NOTHROW(CWwLocalSearch(clientes, distMatrix, 10, 0, 2));
}

TEST_CASE("CWwLocalSearch: No empeora la solución inicial de Clarke & Wright", "[CWwLocalSearch]") {
    // La búsqueda local nunca debe empeorar la solución inicial
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    CWwLocalSearch localSearch(clientes, distMatrix, 10, 0, 2);
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion inicial = heuristica.resolver();
    Solucion mejorada = localSearch.resolver();
    REQUIRE(mejorada.getCostoTotal() <= inicial.getCostoTotal());
}

TEST_CASE("CWwLocalSearch: Mejora la solución en instancia donde es posible", "[CWwLocalSearch]") {
    // Se construye una instancia donde la búsqueda local puede mejorar la solución inicial
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    CWwLocalSearch localSearch(clientes, distMatrix, 10, 0, 2);
    HeuristicaClarkeWright heuristica(clientes, distMatrix, 10, 0, 2);
    Solucion inicial = heuristica.resolver();
    Solucion mejorada = localSearch.resolver();
    REQUIRE(mejorada.getCostoTotal() < inicial.getCostoTotal());
}

TEST_CASE("CWwLocalSearch: Respeta restricciones de capacidad en todas las rutas", "[CWwLocalSearch]") {
    // La solución final nunca debe violar la capacidad máxima de las rutas
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 8), Cliente(2, 8), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    CWwLocalSearch localSearch(clientes, distMatrix, 10, 0, 2);
    Solucion mejorada = localSearch.resolver();
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}

TEST_CASE("CWwLocalSearch: Caso borde con un solo cliente", "[CWwLocalSearch]") {
    // Si solo hay un cliente, la solución debe ser trivial
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 5)};
    vector<vector<double>> distMatrix = {
        {0, 10},
        {10, 0}
    };
    CWwLocalSearch localSearch(clientes, distMatrix, 10, 0, 1);
    Solucion mejorada = localSearch.resolver();
    REQUIRE(mejorada.getRutas().size() == 1);
    REQUIRE(mejorada.getRutas()[0].getClientes().size() == 2); // depósito + cliente
}

TEST_CASE("CWwLocalSearch: Regresión/Aleatorio, siempre respeta factibilidad", "[CWwLocalSearch]") {
    // Prueba aleatoria para asegurar que nunca se violan restricciones
    std::mt19937 rng(42);
    int n = 6;
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
    CWwLocalSearch localSearch(clientes, distMatrix, 10, 0, n);
    Solucion mejorada = localSearch.resolver();
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}
