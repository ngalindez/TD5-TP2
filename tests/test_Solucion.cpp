#include "catch.hpp"
#include "../src/Solucion.h"
#include "../src/Ruta.h"
#include "../src/Cliente.h"
#include <vector>

TEST_CASE("Solucion construction and cost", "[Solucion]") {
    std::vector<Cliente> clientes = {Cliente(2, 5)};
    std::vector<std::vector<double>> distMatrix = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 10, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    // Route: depot -> 2 -> depot
    std::vector<int> clientesRuta = {1, 2, 1};
    Ruta ruta(clientesRuta, capacidad, deposito, distMatrix, clientes);
    std::vector<Ruta> rutas = {ruta};
    Solucion sol(rutas, clientes, distMatrix);
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getCostoTotal() == Approx(20.0));
    REQUIRE(sol.esFactible());
} 