#pragma once
#include "Cliente.h"
#include "Solucion.h"
#include <vector>
#include <unordered_map>

class HeuristicaInsercionCercana {
public:
    HeuristicaInsercionCercana(
        const std::vector<Cliente>& clientes,
        const std::vector<std::vector<double>>& distMatrix,
        const std::unordered_map<int,int>& id2pos,
        int capacidadVehiculo,
        int depotId,
        int numVehiculos);

    Solucion resolver();

private:
    Cliente buscarMasCercano(int desde, const std::vector<bool>& visitado, const std::vector<std::vector<double>>& distMatrix);
    std::vector<Cliente> clientes;
    const std::vector<std::vector<double>>& distMatrix;
    std::unordered_map<int,int> id2pos;
    int capacidadVehiculo;
    int depotId;
    int numVehiculos;
};
