#include "catch.hpp"
#include "../src/Solucion.h"
#include "../src/Ruta.h"
#include "../src/Cliente.h"
#include <vector>

using namespace std;

TEST_CASE("Construcción básica de Solucion", "[Solucion]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    vector<int> clientesRuta = {1, 2, 3, 1};
    Ruta ruta(capacidad, deposito, distMatrix, clientes, clientesRuta);
    vector<Ruta> rutas = {ruta};
    
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getCostoTotal() == Approx(45.0));
    REQUIRE(sol.getCantCamiones() == 1);
    REQUIRE(sol.esFactible());
}

TEST_CASE("Agregar ruta a la solución", "[Solucion]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    Solucion sol(clientes, distMatrix, 2); // Capacidad para 2 camiones
    vector<int> ruta1Clientes = {1, 2, 1};
    vector<int> ruta2Clientes = {1, 3, 1};
    Ruta ruta1(capacidad, deposito, distMatrix, clientes, ruta1Clientes);
    Ruta ruta2(capacidad, deposito, distMatrix, clientes, ruta2Clientes);
    REQUIRE(sol.agregarRuta(ruta1));
    REQUIRE(sol.agregarRuta(ruta2));
    REQUIRE_FALSE(sol.agregarRuta(ruta1)); // No debe permitir más rutas
    REQUIRE(sol.getRutas().size() == 2);
    REQUIRE(sol.getCostoTotal() == Approx(60.0));
    REQUIRE(sol.esFactible());
}

TEST_CASE("Remover ruta de la solución", "[Solucion]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    vector<int> ruta1Clientes = {1, 2, 1};
    vector<int> ruta2Clientes = {1, 3, 1};
    Ruta ruta1(capacidad, deposito, distMatrix, clientes, ruta1Clientes);
    Ruta ruta2(capacidad, deposito, distMatrix, clientes, ruta2Clientes);
    vector<Ruta> rutas = {ruta1, ruta2};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    REQUIRE(sol.getRutas().size() == 2);
    REQUIRE(sol.removerRuta(0)); // ahora recibe índice
    REQUIRE(sol.getRutas().size() == 1);
    REQUIRE(sol.getCostoTotal() == Approx(40.0));
    REQUIRE_FALSE(sol.removerRuta(5)); // índice inválido
}

TEST_CASE("Factibilidad de la solución con exceso de rutas", "[Solucion]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4), Cliente(4, 3)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0, 0},
        {0, 0, 10, 20, 30},
        {0, 10, 0, 15, 25},
        {0, 20, 15, 0, 12},
        {0, 30, 25, 12, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    vector<int> ruta1Clientes = {1, 2, 1};
    vector<int> ruta2Clientes = {1, 3, 1};
    vector<int> ruta3Clientes = {1, 4, 1};
    Ruta ruta1(capacidad, deposito, distMatrix, clientes, ruta1Clientes);
    Ruta ruta2(capacidad, deposito, distMatrix, clientes, ruta2Clientes);
    Ruta ruta3(capacidad, deposito, distMatrix, clientes, ruta3Clientes);
    vector<Ruta> rutas = {ruta1, ruta2, ruta3};
    Solucion sol(clientes, distMatrix, 2, rutas); // Capacidad solo para 2 camiones
    REQUIRE(sol.getRutas().size() == 3);
    REQUIRE_FALSE(sol.esFactible());
}

TEST_CASE("Obtener clientes y matriz de distancias de la solución", "[Solucion]") {
    vector<Cliente> clientes = {Cliente(2, 5), Cliente(3, 4)};
    vector<vector<double>> distMatrix = {
        {0, 0, 0, 0},
        {0, 0, 10, 20},
        {0, 10, 0, 15},
        {0, 20, 15, 0}
    };
    int capacidad = 10;
    int deposito = 1;
    vector<int> clientesRuta = {1, 2, 3, 1};
    Ruta ruta(capacidad, deposito, distMatrix, clientes, clientesRuta);
    vector<Ruta> rutas = {ruta};
    Solucion sol(clientes, distMatrix, rutas.size(), rutas);
    const vector<Cliente>& clientesSol = sol.getClientes();
    REQUIRE(clientesSol.size() == 2);
    REQUIRE(clientesSol[0].getId() == 2);
    REQUIRE(clientesSol[1].getId() == 3);
    const vector<vector<double>>& mat = sol.getDistMatrix();
    REQUIRE(mat[1][2] == 10);
    REQUIRE(mat[2][3] == 15);
}
