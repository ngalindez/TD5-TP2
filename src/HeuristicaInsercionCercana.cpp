#include "HeuristicaInsercionCercana.h"
#include <limits>
#include <algorithm>

HeuristicaInsercionCercana::HeuristicaInsercionCercana( const vector<Cliente>& clientes,
                                                        const vector<vector<double>>& distMatrix,
                                                        int capacidadVehiculo,
                                                        int depotId,
                                                        int numVehiculos)
    : clientes(clientes),
      distMatrix(distMatrix),
      capacidadVehiculo(capacidadVehiculo),
      depotId(depotId),
      numVehiculos(numVehiculos)
{}

Solucion HeuristicaInsercionCercana::resolver() {
    vector<Ruta> rutas;
    Solucion sol(clientes, distMatrix, numVehiculos);

    // Encontrar el ID máximo para dimensionar el vector visitado
    int maxId = depotId;
    for (const auto& c : clientes) {
        if (c.getId() > maxId) maxId = c.getId();
    }

    // Vector de visitados por ID (asegura tamaño suficiente)
    vector<bool> visitado(maxId + 1, false);
    if (depotId >= 0 && depotId < (int)visitado.size())
        visitado[depotId] = true; // el depósito está siempre visitado

    size_t cantidadVisitados = 0;
    size_t cantidadClientes = clientes.size(); // total sin contar el depósito

    while (cantidadVisitados < cantidadClientes) {
        Ruta ruta(capacidadVehiculo, depotId, distMatrix, clientes);
        int actual = depotId; // depósito
        int carga = 0;
        bool added = false;

        while (true) {
            Cliente masCercano = buscarMasCercano(actual, visitado);

            if (masCercano.getId() == 0) break; // No hay cliente válido

            int demanda = masCercano.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(masCercano.getId());
                if (masCercano.getId() >= 0 && masCercano.getId() < (int)visitado.size())
                visitado[masCercano.getId()] = true;
                carga += demanda;
                actual = masCercano.getId();
                cantidadVisitados++;
                added = true;
            } else {
                break; // No entra más
            }
        }
        // Solo agregar rutas que realmente tengan clientes
        if (added)
        rutas.push_back(ruta);
        else
            break; // Previene bucles infinitos si no se puede agregar ningún cliente
    }

    for (const auto& ruta : rutas) {
        sol.agregarRuta(ruta);
    }

    return sol;
}

Cliente HeuristicaInsercionCercana::buscarMasCercano(int desde, const vector<bool>& visitado) {
    double mejorDist = numeric_limits<double>::max();
    Cliente mejor(0, 0); // ID 0 → marcador inválido

    for (const Cliente& c : clientes) {
        if (c.getId() >= 0 && c.getId() < (int)visitado.size() && !visitado[c.getId()]) {
            double dist = distMatrix[desde][c.getId()];
            if (dist < mejorDist) {
                mejorDist = dist;
                mejor = c;
            }
        }
    }

    return mejor;
}
