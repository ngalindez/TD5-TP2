#include "catch.hpp"
#include "../src/OperadorSwap.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <algorithm>

using namespace std;

TEST_CASE("OperadorSwap: Construcción básica con solución válida", "[OperadorSwap]") {
    // Prueba de construcción básica
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1(10, 0, distMatrix, clientes, {0, 1, 0});
    Ruta ruta2(10, 0, distMatrix, clientes, {0, 2, 0});
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    REQUIRE_NOTHROW(OperadorSwap(sol));
}

TEST_CASE("OperadorSwap: No realiza swap si no mejora la solución", "[OperadorSwap]") {
    // Si el swap no mejora el costo, la solución debe permanecer igual
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1(10, 0, distMatrix, clientes, {0, 1, 0});
    Ruta ruta2(10, 0, distMatrix, clientes, {0, 2, 0});
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    OperadorSwap op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser igual al original
    REQUIRE(mejorada.getCostoTotal() == sol.getCostoTotal());
}

TEST_CASE("OperadorSwap: Realiza swap que mejora la solución", "[OperadorSwap]") {
    // Se construye una instancia donde el swap reduce el costo total
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    Ruta ruta1(10, 0, distMatrix, clientes, {0, 1, 2, 0});
    Ruta ruta2(10, 0, distMatrix, clientes, {0, 3, 0});
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    OperadorSwap op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser menor al original
    REQUIRE(mejorada.getCostoTotal() < sol.getCostoTotal());
}

TEST_CASE("OperadorSwap: Respeta restricciones de capacidad", "[OperadorSwap]") {
    // El swap no debe violar la capacidad máxima de las rutas
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 8), Cliente(2, 8), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    Ruta ruta1(10, 0, distMatrix, clientes, {0, 1, 3, 0});
    Ruta ruta2(10, 0, distMatrix, clientes, {0, 2, 0});
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    OperadorSwap op(sol);
    Solucion mejorada = op.aplicar();
    // Ninguna ruta debe exceder la capacidad
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}

TEST_CASE("OperadorSwap: Caso borde con una sola ruta", "[OperadorSwap]") {
    // Si solo hay una ruta, no debe intentar swaps
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1(10, 0, distMatrix, clientes, {0, 1, 2, 0});
    vector<Ruta> rutas = {ruta1};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    OperadorSwap op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser igual al original
    REQUIRE(mejorada.getCostoTotal() == sol.getCostoTotal());
}

TEST_CASE("OperadorSwap: Regresión/Aleatorio, siempre respeta factibilidad", "[OperadorSwap]") {
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
    // Crear rutas simples
    vector<Ruta> rutas;
    for (int i = 1; i <= n; ++i) {
        rutas.push_back(Ruta(10, 0, distMatrix, clientes, {0, i, 0}));
    }
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    OperadorSwap op(sol);
    Solucion mejorada = op.aplicar();
    // Todas las rutas deben ser factibles
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}
