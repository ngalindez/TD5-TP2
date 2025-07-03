#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/Cliente.h"

TEST_CASE("Cliente basic functionality", "[Cliente]") {
    Cliente c(5, 10);
    REQUIRE(c.getId() == 5);
    REQUIRE(c.getDemand() == 10);
} 