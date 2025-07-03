#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/Cliente.h"
#include <stdexcept>

using namespace std;

TEST_CASE("Funcionalidad básica de clase Cliente", "[Cliente]") {
    Cliente c(5, 10);
    REQUIRE(c.getId() == 5);
    REQUIRE(c.getDemand() == 10);
} 

TEST_CASE("Clase cliente con id negativo", "[Cliente]") {
    REQUIRE_THROWS_AS(Cliente(-1, 10), invalid_argument);
}

TEST_CASE("Clase cliente con demanda negativo", "[Cliente]") {
    REQUIRE_THROWS_AS(Cliente(1, -10), invalid_argument);
}