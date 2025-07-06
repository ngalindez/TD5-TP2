#include "catch.hpp"
#include "../src/CWwLocalSearch.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>

using namespace std;

// Helper: apply swap until no further improvement
Solucion aplicarSwapHastaMejorar(const Solucion& inicial) {
    Solucion actual = inicial;
    while (true) {
        OperadorSwap op(actual);
        Solucion mejorada = op.aplicar();
        if (mejorada.getCostoTotal() < actual.getCostoTotal())
            actual = mejorada;
        else
            break;
    }
    return actual;
}

// Helper: apply relocate until no further improvement
Solucion aplicarRelocateHastaMejorar(const Solucion& inicial) {
    Solucion actual = inicial;
    while (true) {
        OperadorRelocate op(actual);
        Solucion mejorada = op.aplicar();
        if (mejorada.getCostoTotal() < actual.getCostoTotal())
            actual = mejorada;
        else
            break;
    }
    return actual;
}

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
    REQUIRE(mejorada.getRutas()[0].getCantidadClientes() == 1); // solo el cliente (sin depósitos)
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

TEST_CASE("CWwLocalSearch: Mejora intra-ruta significativa", "[CWwLocalSearch]") {
    vector<Cliente> clientes = {Cliente(1, 2), Cliente(2, 2), Cliente(3, 2), Cliente(4, 2)};
    vector<vector<double>> distMatrix = {
        { 0, 1, 1, 1, 1 },
        { 1, 0, 2, 2, 100 },
        { 1, 2, 0, 2, 2 },
        { 1, 2, 2, 0, 2 },
        { 1, 100, 2, 2, 0 }
    };
    int capacidad = 8;
    int depotId = 0;
    int numVehiculos = 1;
    // Solución inicial muy mala: 0-1-4-3-2-0
    vector<int> rutaMala = {0, 1, 4, 3, 2, 0};
    vector<Ruta> rutas = {Ruta(capacidad, depotId, distMatrix, clientes, rutaMala)};
    Solucion suboptima(clientes, distMatrix, rutas.size(), rutas);
    std::cout << "Costo subóptimo: " << suboptima.getCostoTotal() << std::endl;
    for (const auto& ruta : suboptima.getRutas()) {
        std::cout << "Ruta subóptima: ";
        for (int id : ruta.getClientes()) std::cout << id << " ";
        std::cout << std::endl;
    }
    // Aplicar búsqueda local (swap)
    Solucion mejorada = aplicarSwapHastaMejorar(suboptima);
    std::cout << "Costo mejorado: " << mejorada.getCostoTotal() << std::endl;
    for (const auto& ruta : mejorada.getRutas()) {
        std::cout << "Ruta mejorada: ";
        for (int id : ruta.getClientes()) std::cout << id << " ";
        std::cout << std::endl;
    }
    REQUIRE(mejorada.getCostoTotal() <= suboptima.getCostoTotal());
    REQUIRE(suboptima.getCostoTotal() > 100);
}

TEST_CASE("CWwLocalSearch: Mejora significativa en rutas con costos desbalanceados", "[CWwLocalSearch]") {
    // Depósito: 0
    // Clientes: 1, 2, 3, 4
    // Demanda: 1 cada uno
    // Capacidad: 2
    // Matriz de distancias:
    //    0  1  2  3  4
    // 0 [0, 2, 2, 10, 10]
    // 1 [2, 0, 1, 10, 10]
    // 2 [2, 1, 0, 10, 10]
    // 3 [10,10,10, 0, 1]
    // 4 [10,10,10, 1, 0]
    vector<Cliente> clientes = {Cliente(1, 1), Cliente(2, 1), Cliente(3, 1), Cliente(4, 1)};
    vector<vector<double>> distMatrix = {
        {0, 2, 2, 10, 10},
        {2, 0, 1, 10, 10},
        {2, 1, 0, 10, 10},
        {10,10,10, 0, 1},
        {10,10,10, 1, 0}
    };
    int capacidad = 2;
    int depotId = 0;
    int numVehiculos = 2;
    // Solución inicial: Ruta 1: 0-1-2-0, Ruta 2: 0-3-4-0
    vector<int> ruta1 = {0, 1, 2, 0};
    vector<int> ruta2 = {0, 3, 4, 0};
    vector<Ruta> rutas = {
        Ruta(capacidad, depotId, distMatrix, clientes, ruta1),
        Ruta(capacidad, depotId, distMatrix, clientes, ruta2)
    };
    Solucion suboptima(clientes, distMatrix, rutas.size(), rutas);
    std::cout << "Costo subóptimo: " << suboptima.getCostoTotal() << std::endl;
    for (const auto& ruta : suboptima.getRutas()) {
        std::cout << "Ruta subóptima: ";
        for (int id : ruta.getClientes()) std::cout << id << " ";
        std::cout << std::endl;
    }
    // Aplicar búsqueda local (swap y relocate)
    Solucion trasSwap = aplicarSwapHastaMejorar(suboptima);
    Solucion mejorada = aplicarRelocateHastaMejorar(trasSwap);
    std::cout << "Costo mejorado: " << mejorada.getCostoTotal() << std::endl;
    for (const auto& ruta : mejorada.getRutas()) {
        std::cout << "Ruta mejorada: ";
        for (int id : ruta.getClientes()) std::cout << id << " ";
        std::cout << std::endl;
    }
    REQUIRE(mejorada.getCostoTotal() <= suboptima.getCostoTotal());
    REQUIRE(suboptima.getCostoTotal() == 26);
}

TEST_CASE("CWwLocalSearch: Swap mejora claramente la solución", "[CWwLocalSearch]") {
    // Dos rutas: 0-1-3-0 y 0-2-4-0, pero 1 y 2 están más cerca entre sí, igual 3 y 4
    // Swapping 2 and 3 should reduce the total cost
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 1), Cliente(2, 1), Cliente(3, 1), Cliente(4, 1)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10, 10},
        {1, 0, 1, 10, 10},
        {10, 1, 0, 1, 10},
        {10, 10, 1, 0, 1},
        {10, 10, 10, 1, 0}
    };
    int capacidad = 2;
    int depotId = 0;
    int numVehiculos = 2;
    vector<int> ruta1 = {0, 1, 3, 0};
    vector<int> ruta2 = {0, 2, 4, 0};
    vector<Ruta> rutas = {
        Ruta(capacidad, depotId, distMatrix, clientes, ruta1),
        Ruta(capacidad, depotId, distMatrix, clientes, ruta2)
    };
    Solucion suboptima(clientes, distMatrix, rutas.size(), rutas);
    Solucion mejorada = aplicarSwapHastaMejorar(suboptima);
    // The optimal solution is 0-1-2-0 and 0-3-4-0, with much lower cost
    REQUIRE(mejorada.getCostoTotal() < suboptima.getCostoTotal());
}