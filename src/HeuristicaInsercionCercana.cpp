#include "HeuristicaInsercionCercana.h"
#include <limits>
#include <algorithm>
#include <iostream>

HeuristicaInsercionCercana::HeuristicaInsercionCercana(
    const std::vector<Cliente>& clientes,
    const std::vector<std::vector<double>>& distMatrix,
    const std::unordered_map<int,int>& id2pos,
    int capacidadVehiculo,
    int depotId,
    int numVehiculos)
    : clientes(clientes),
      distMatrix(distMatrix),
      id2pos(id2pos),
      capacidadVehiculo(capacidadVehiculo),
      depotId(depotId),
      numVehiculos(numVehiculos)
{}

Solucion HeuristicaInsercionCercana::resolver() {
    std::vector<Ruta> rutas;

    // Use the original matrix directly (already indexed by node IDs)
    std::vector<std::vector<double>> idDistMatrix = distMatrix;

    Solucion sol(clientes, idDistMatrix, numVehiculos);

    // Prepare visited flags by client ID
    int maxId = static_cast<int>(idDistMatrix.size()) - 1;
    std::vector<bool> visitado(maxId + 1, false);
    visitado.at(depotId) = true;

    size_t cantidadVisitados = 0;
    size_t cantidadClientes = clientes.size();

    while (cantidadVisitados < cantidadClientes) {
        Ruta ruta(capacidadVehiculo, depotId, idDistMatrix, clientes);
        int actual = depotId;
        int carga = 0;
        bool added = false;

        while (true) {
            Cliente masCercano = buscarMasCercano(actual, visitado, idDistMatrix);
            if (masCercano.getId() == depotId) {
                break;
            }
            int demanda = masCercano.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(masCercano.getId());
                visitado.at(masCercano.getId()) = true;
                carga += demanda;
                actual = masCercano.getId();
                cantidadVisitados++;
                added = true;
            } else {
                break;
            }
        }
        if (added) {
            rutas.push_back(ruta);
        } else {
            break;
        }
    }

    for (const auto& ruta : rutas) {
        sol.agregarRuta(ruta);
    }

    return sol;
}

Cliente HeuristicaInsercionCercana::buscarMasCercano(
    int desde,
    const std::vector<bool>& visitado,
    const std::vector<std::vector<double>>& idDistMatrix) {
    double mejorDist = std::numeric_limits<double>::max();
    Cliente mejor(depotId, 0);

    for (const Cliente& c : clientes) {
        int cid = c.getId();
        if (!visitado[cid] && cid != depotId) {
            double dist = idDistMatrix[desde][cid];
            if (dist < mejorDist) {
                mejorDist = dist;
                mejor = c;
            }
        }
    }
    return mejor;
}
