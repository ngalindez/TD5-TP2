#include "HeuristicaInsercionCercana.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <set>

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
    // 1) Reconstruir matriz de distancias ID→ID
    int maxId = depotId;
    for (const auto &p : id2pos) maxId = std::max(maxId, p.first);
    std::vector<std::vector<double>> idDist(maxId+1, std::vector<double>(maxId+1, 0.0));
    for (const auto &p1 : id2pos) {
        for (const auto &p2 : id2pos) {
            idDist[p1.first][p2.first] = distMatrix[p1.second][p2.second];
        }
    }

    // 2) Inicializar solución
    Solucion sol(clientes, idDist, numVehiculos);

    // 3) Conjunto de clientes no visitados (IDs)
    std::set<int> unvisited;
    for (const auto &c : clientes) unvisited.insert(c.getId());

    // 4) Construir rutas hasta agotar clientes o vehículos
    while (!unvisited.empty() &&
           sol.getRutas().size() < static_cast<size_t>(numVehiculos)) {
        std::vector<int> seq;          // Secuencia de clientes para la ruta
        int carga = 0;                 // Demanda actual

        // 4.1) Semilla: cliente más cercano al depósito
        int firstId = -1;
        double bestD = std::numeric_limits<double>::max();
        for (int id : unvisited) {
            double d = idDist[depotId][id];
            if (d < bestD) { bestD = d; firstId = id; }
        }
        // Si ninguno cabe en un camión, abortar
        int dem = 0;
        for (const auto &c : clientes) if (c.getId() == firstId) dem = c.getDemand();
        if (dem > capacidadVehiculo) break;
        // Agregar semilla
        seq.push_back(firstId);
        carga += dem;
        unvisited.erase(firstId);

        // 4.2) Inserción de los demás clientes
        while (true) {
            // Buscar cliente no visitado cuyo nodo más cercano en ruta esté a distancia mínima
            int candId = -1;
            double minDist = std::numeric_limits<double>::max();
            for (int id : unvisited) {
                // distancia mínima a cualquier nodo en ruta o depósito
                double d0 = idDist[depotId][id];
                double dmin = d0;
                for (int v : seq) dmin = std::min(dmin, idDist[v][id]);
                if (dmin < minDist) { minDist = dmin; candId = id; }
            }
            if (candId < 0) break;
            // Demanda del candidato
            int demC = 0;
            for (const auto &c : clientes) if (c.getId() == candId) demC = c.getDemand();
            if (carga + demC > capacidadVehiculo) break;

            // Encontrar mejor posición de inserción en seq
            size_t bestPos = 0;
            double bestInc = std::numeric_limits<double>::max();
            size_t n = seq.size();
            for (size_t i = 0; i <= n; ++i) {
                int prev = (i==0 ? depotId : seq[i-1]);
                int next = (i==n ? depotId : seq[i]);
                double inc = idDist[prev][candId] + idDist[candId][next] - idDist[prev][next];
                if (inc < bestInc) { bestInc = inc; bestPos = i; }
            }
            // Insertar
            seq.insert(seq.begin() + bestPos, candId);
            carga += demC;
            unvisited.erase(candId);
        }

        // 4.3) Crear y agregar ruta a la solución
        Ruta ruta(capacidadVehiculo, depotId, idDist, clientes, seq);
        if (!sol.agregarRuta(ruta)) {
            // Excedió vehículo
            break;
        }
    }

    return sol;
}

Cliente HeuristicaInsercionCercana::buscarMasCercano(
    int desde,
    const std::vector<bool>&,
    const std::vector<std::vector<double>>&) {
    // No usado en esta implementación
    return Cliente(depotId, 0);
}