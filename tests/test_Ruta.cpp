#include "catch.hpp"
#include "../src/Ruta.h"
#include "../src/Cliente.h"
#include <vector>

TEST_CASE("Ruta basic construction and methods", "[Ruta]") {
    // Setup: depot id = 1, one client id = 2
    std::vector<Cliente> clientes = {Cliente(2, 5)};
    std::vector<std::vector<double>> distMatrix = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 10, 0}
    };
    int capacidad = 10;
    int deposito = 1;

    Ruta ruta(capacidad, deposito, distMatrix, clientes);
    REQUIRE(ruta.getClientes().size() == 1); // Only depot at start
    ruta.agregarCliente(2);
    REQUIRE(ruta.getClientes().size() == 2); // Depot + client
    REQUIRE(ruta.getDemandaActual() == 5);
    ruta.agregarCliente(2); // Should not add again (capacity check)
    REQUIRE(ruta.getDemandaActual() == 5); // Still 5
    // Add depot at end for cost calculation
    ruta.agregarCliente(deposito);
    double cost = ruta.calcularCosto();
    REQUIRE(cost == Approx(20.0)); // depot->2->depot
} 