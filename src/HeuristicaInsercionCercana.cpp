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

    // Vector de visitados por ID
    vector<bool> visitado(distMatrix.size(), false);
    visitado[1] = true; // el depósito está siempre visitado

    size_t cantidadVisitados = 0;
    size_t cantidadClientes = clientes.size(); // total sin contar el depósito

    while (cantidadVisitados < cantidadClientes) {
        Ruta ruta(capacidadVehiculo, 1, distMatrix, clientes);
        int actual = 1; // depósito
        int carga = 0;

        while (true) {
            Cliente masCercano = buscarMasCercano(actual, visitado);

            if (masCercano.getId() == -1) break; // No hay cliente válido

            int demanda = masCercano.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(masCercano.getId());
                visitado[masCercano.getId()] = true;
                carga += demanda;
                actual = masCercano.getId();
                cantidadVisitados++;
            } else {
                break; // No entra más
            }
        }

        rutas.push_back(ruta);
    }

    return Solucion(rutas, clientes, distMatrix);
}


Cliente HeuristicaInsercionCercana::buscarMasCercano(int desde, const vector<bool>& visitado) {
    double mejorDist = numeric_limits<double>::max();
    Cliente mejor(-1, 0); // ID -1 → marcador inválido

    for (const Cliente& c : clientes) {
        if (!visitado[c.getId()]) {
            double dist = distMatrix[desde][c.getId()];
            if (dist < mejorDist) {
                mejorDist = dist;
                mejor = c;
            }
        }
    }

    return mejor;
}
