#include "catch.hpp"
#include "../src/OperadorRelocate.h"
#include "../src/Cliente.h"
#include "../src/Ruta.h"
#include "../src/Solucion.h"
#include <vector>
#include <algorithm>

using namespace std;

TEST_CASE("OperadorRelocate: Construcción básica con solución válida", "[OperadorRelocate]") {
    // Prueba de construcción básica
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1({0, 1, 0}, 10, 0, distMatrix, clientes);
    Ruta ruta2({0, 2, 0}, 10, 0, distMatrix, clientes);
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(rutas, clientes, distMatrix);
    REQUIRE_NOTHROW(OperadorRelocate(sol));
}

TEST_CASE("OperadorRelocate: No realiza relocate si no mejora la solución", "[OperadorRelocate]") {
    // Si el relocate no mejora el costo, la solución debe permanecer igual
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1({0, 1, 0}, 10, 0, distMatrix, clientes);
    Ruta ruta2({0, 2, 0}, 10, 0, distMatrix, clientes);
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(rutas, clientes, distMatrix);
    OperadorRelocate op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser igual al original
    REQUIRE(mejorada.getCostoTotal() == sol.getCostoTotal());
}

TEST_CASE("OperadorRelocate: Realiza relocate que mejora la solución", "[OperadorRelocate]") {
    // Se construye una instancia donde el relocate reduce el costo total
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    Ruta ruta1({0, 1, 2, 0}, 10, 0, distMatrix, clientes);
    Ruta ruta2({0, 3, 0}, 10, 0, distMatrix, clientes);
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(rutas, clientes, distMatrix);
    OperadorRelocate op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser menor al original
    REQUIRE(mejorada.getCostoTotal() < sol.getCostoTotal());
}

TEST_CASE("OperadorRelocate: Respeta restricciones de capacidad", "[OperadorRelocate]") {
    // El relocate no debe violar la capacidad máxima de las rutas
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 8), Cliente(2, 8), Cliente(3, 2)};
    vector<vector<double>> distMatrix = {
        {0, 1, 10, 10},
        {1, 0, 10, 2},
        {10, 10, 0, 1},
        {10, 2, 1, 0}
    };
    Ruta ruta1({0, 1, 3, 0}, 10, 0, distMatrix, clientes);
    Ruta ruta2({0, 2, 0}, 10, 0, distMatrix, clientes);
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(rutas, clientes, distMatrix);
    OperadorRelocate op(sol);
    Solucion mejorada = op.aplicar();
    // Ninguna ruta debe exceder la capacidad
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}

TEST_CASE("OperadorRelocate: Caso borde con una sola ruta", "[OperadorRelocate]") {
    // Si solo hay una ruta, no debe intentar relocate
    vector<Cliente> clientes = {Cliente(0, 0), Cliente(1, 3), Cliente(2, 4)};
    vector<vector<double>> distMatrix = {
        {0, 5, 6},
        {5, 0, 2},
        {6, 2, 0}
    };
    Ruta ruta1({0, 1, 2, 0}, 10, 0, distMatrix, clientes);
    vector<Ruta> rutas = {ruta1};
    Solucion sol(rutas, clientes, distMatrix);
    OperadorRelocate op(sol);
    Solucion mejorada = op.aplicar();
    // El costo debe ser igual al original
    REQUIRE(mejorada.getCostoTotal() == sol.getCostoTotal());
}

TEST_CASE("OperadorRelocate: Regresión/Aleatorio, siempre respeta factibilidad", "[OperadorRelocate]") {
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
        rutas.push_back(Ruta({0, i, 0}, 10, 0, distMatrix, clientes));
    }
    Solucion sol(rutas, clientes, distMatrix);
    OperadorRelocate op(sol);
    Solucion mejorada = op.aplicar();
    // Todas las rutas deben ser factibles
    for (const auto& ruta : mejorada.getRutas()) {
        REQUIRE(ruta.getDemandaActual() <= ruta.getCapacidadMaxima());
        REQUIRE(ruta.esFactible());
    }
}
