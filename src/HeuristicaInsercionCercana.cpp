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
    
    // Create a distance matrix indexed by client IDs instead of positions
    int maxId = depotId;
    for (const auto& pair : id2pos) {
        maxId = std::max(maxId, pair.first);
    }
    std::vector<std::vector<double>> idDistMatrix(maxId + 1, std::vector<double>(maxId + 1, 0.0));
    
    // Copy distances from position-based matrix to ID-based matrix
    for (const auto& pair1 : id2pos) {
        for (const auto& pair2 : id2pos) {
            idDistMatrix[pair1.first][pair2.first] = distMatrix[pair1.second][pair2.second];
        }
    }
    
    Solucion sol(clientes, idDistMatrix, numVehiculos);

    // Find the maximum position in id2pos to size the visited vector correctly
    int maxPos = 0;
    for (const auto& pair : id2pos) {
        maxPos = std::max(maxPos, pair.second);
    }
    std::vector<bool> visitado(maxPos + 1, false);
    visitado.at(id2pos.at(depotId)) = true;

    size_t cantidadVisitados = 0;
    size_t cantidadClientes = clientes.size();



    while (cantidadVisitados < cantidadClientes) {
        Ruta ruta(capacidadVehiculo, depotId, idDistMatrix, clientes);
        int actual = depotId;
        int carga = 0;
        bool added = false;

            while (true) {
        Cliente masCercano = buscarMasCercano(actual, visitado);

        if (masCercano.getId() == depotId) break;

            int demanda = masCercano.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(masCercano.getId());
                visitado.at(id2pos.at(masCercano.getId())) = true;
                carga += demanda;
                actual = masCercano.getId();
                cantidadVisitados++;
                added = true;
            } else {
                break;
            }
        }

        if (added)
            rutas.push_back(ruta);
        else
            break;
    }

    for (const auto& ruta : rutas) {
        sol.agregarRuta(ruta);
    }

    return sol;
}

Cliente HeuristicaInsercionCercana::buscarMasCercano(int desde, const std::vector<bool>& visitado) {
    double mejorDist = std::numeric_limits<double>::max();
    Cliente mejor(depotId, 0);

    for (const Cliente& c : clientes) {
        int pos = id2pos.at(c.getId());
        if (!visitado[pos] && c.getId() != depotId) {
            double dist = distMatrix.at(id2pos.at(desde)).at(pos);
            if (dist < mejorDist) {
                mejorDist = dist;
                mejor = c;
            }
        }
    }

    return mejor;
}
