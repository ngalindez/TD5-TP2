#include "catch.hpp"
#include "../src/Ruta.h"
#include "../src/Cliente.h"
#include <vector>

using namespace std;

TEST_CASE("Construcción básica de Ruta", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 10, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    Ruta ruta(capacidad, deposito, distMatrix, clientes);
    REQUIRE(ruta.getClientes().size() == 1); // Solo depósito al inicio
    REQUIRE(ruta.getDemandaActual() == 0);
    REQUIRE(ruta.getCapacidadMaxima() == 10);
    REQUIRE(ruta.getIdDeposito() == 1);
}

TEST_CASE("Agregar cliente a la ruta", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 10, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.agregarCliente(2);
    REQUIRE(ruta.getClientes().size() == 2); // depósito + cliente
    REQUIRE(ruta.getDemandaActual() == 5);
}

TEST_CASE("No agregar cliente si excede capacidad", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 8), Cliente(3, 5)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.agregarCliente(2); // demanda 8
    ruta.agregarCliente(3); // demanda 5, no debe agregarse
    REQUIRE(ruta.getDemandaActual() == 8);
    REQUIRE(ruta.getClientes().size() == 2); // depósito + cliente 2
}

TEST_CASE("Eliminar cliente de la ruta", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.agregarCliente(2);
    ruta.agregarCliente(3);
    REQUIRE(ruta.getDemandaActual() == 9);
    ruta.eliminarCliente(2);
    REQUIRE(ruta.getDemandaActual() == 4);
    REQUIRE(ruta.getClientes().size() == 2); // depósito + cliente 3
}

TEST_CASE("Calcular costo de la ruta", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    Ruta ruta(15, 1, distMatrix, clientes);
    ruta.agregarCliente(2);
    ruta.agregarCliente(3);
    // La ruta es: 1 -> 2 -> 3 -> 1
    ruta.agregarCliente(1); // Para cerrar el ciclo
    double costo = ruta.calcularCosto();
    // 1->2 (10) + 2->3 (15) + 3->1 (20) = 45
    REQUIRE(costo == Approx(45.0));
}

TEST_CASE("Factibilidad de la ruta", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 6)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.agregarCliente(2);
    REQUIRE(ruta.esFactible());
    ruta.agregarCliente(3); // Excede capacidad
    REQUIRE_FALSE(ruta.esFactible());
}

TEST_CASE("No eliminar cliente inexistente", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 5)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0},
        {0, 0, 10},
        {0, 10, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.eliminarCliente(2); // No debe afectar nada
    REQUIRE(ruta.getDemandaActual() == 0);
    REQUIRE(ruta.getClientes().size() == 1); // Solo depósito
}

TEST_CASE("Agregar y eliminar múltiples clientes", "[Ruta]") {
    vector<Cliente> clientes = {Cliente(2, 3), Cliente(3, 4), Cliente(4, 2)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0, 0},
        {0, 0, 10, 20, 30},
        {0, 10, 0, 15, 25},
        {0, 20, 15, 0, 12},
        {0, 30, 25, 12, 0}
    };
    Ruta ruta(10, 1, distMatrix, clientes);
    ruta.agregarCliente(2);
    ruta.agregarCliente(3);
    ruta.agregarCliente(4);
    REQUIRE(ruta.getDemandaActual() == 9);
    ruta.eliminarCliente(3);
    REQUIRE(ruta.getDemandaActual() == 5);
    ruta.eliminarCliente(2);
    REQUIRE(ruta.getDemandaActual() == 2);
    ruta.eliminarCliente(4);
    REQUIRE(ruta.getDemandaActual() == 0);
    REQUIRE(ruta.getClientes().size() == 1); // Solo depósito
} 