#include "GRASP.h"
#include "OperadorSwap.h"
#include "OperadorRelocate.h"
#include <algorithm>
#include <random>
#include <limits>

using namespace std;

GRASP::GRASP(
    const vector<Cliente>& clientes,
    const vector<vector<double>>& distMatrix,
    const unordered_map<int, int>& id2pos,
    int capacidadVehiculo,
    int depotId,
    int numVehiculos,
    int numIter,
    int kRCL)
    : clientes(clientes),
      distMatrix(distMatrix),
      id2pos(id2pos),
      capacidadVehiculo(capacidadVehiculo),
      depotId(depotId),
      numVehiculos(numVehiculos),
      numIter(numIter),
      kRCL(kRCL)
{}

Solucion GRASP::resolver() {
    Solucion mejorSol = Solucion(clientes, distMatrix, numVehiculos);
    double mejorCosto = numeric_limits<double>::max();
    bool primerIter = true;

    for (int iter = 0; iter < numIter; ++iter) {
        Solucion sol = construirConRCL(kRCL);

        OperadorSwap swapOp(sol);
        sol = swapOp.aplicar();

        OperadorRelocate relocateOp(sol);
        sol = relocateOp.aplicar();

        // ✅ Check robusto antes de comparar costo
        if (!sol.esFactible() || !sol.vistoTodos())
            continue;

        if (primerIter || sol.getCostoTotal() < mejorCosto) {
            mejorSol = sol;
            mejorCosto = sol.getCostoTotal();
            primerIter = false;
        }
    }

    return mejorSol;
}



Solucion GRASP::construirConRCL(int k) {
    vector<Ruta> rutas;

    // Crear matriz de distancias por ID
    int maxId = depotId;
    for (const auto& pair : id2pos) {
        maxId = max(maxId, pair.first);
    }
    vector<vector<double>> idDistMatrix(maxId + 1, vector<double>(maxId + 1, 0.0));

    for (const auto& pair1 : id2pos) {
        for (const auto& pair2 : id2pos) {
            idDistMatrix[pair1.first][pair2.first] = distMatrix[pair1.second][pair2.second];
        }
    }

    Solucion sol(clientes, idDistMatrix, numVehiculos);

    // Vector de visitados
    int maxPos = 0;
    for (const auto& pair : id2pos) {
        maxPos = max(maxPos, pair.second);
    }
    vector<bool> visitado(maxPos + 1, false);
    visitado.at(id2pos.at(depotId)) = true;

    while (true) {
        // Verificar si quedan clientes sin visitar
        bool quedanClientes = false;
        for (const Cliente& c : clientes) {
            if (!visitado.at(id2pos.at(c.getId()))) {
                quedanClientes = true;
                break;
            }
        }
        if (!quedanClientes)
            break;

        Ruta ruta(capacidadVehiculo, depotId, idDistMatrix, clientes);
        int actual = depotId;
        int carga = 0;
        bool added = false;

        while (true) {
            vector<Cliente> KMasCercanos = buscarKcercanos(actual, visitado, k);
            Cliente masCercano = elegirClienteRandom(KMasCercanos);

            int i = 0;
            while (masCercano.getId() == depotId && i < kRCL) {
                masCercano = elegirClienteRandom(KMasCercanos);
                i++;
            }
            if (masCercano.getId() == depotId)
                break;

            int demanda = masCercano.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(masCercano.getId());
                visitado.at(id2pos.at(masCercano.getId())) = true;
                carga += demanda;
                actual = masCercano.getId();
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

vector<Cliente> GRASP::buscarKcercanos(int desde, const vector<bool>& visitado, int k) {
    vector<pair<double, Cliente>> candidatos;

    // Recorrer todos los clientes
    for (const Cliente& c : clientes) {
        int pos = id2pos.at(c.getId());

        // Solo incluir si NO está visitado y no es el depósito
        if (!visitado[pos] && c.getId() != depotId) {
            double dist = distMatrix.at(id2pos.at(desde)).at(pos);
            candidatos.emplace_back(dist, c);
        }
    }

    // Si no hay candidatos, devolver lista vacía
    if (candidatos.empty())
        return {};

    // Ordenar por distancia (de menor a mayor)
    sort(candidatos.begin(), candidatos.end(),
        [](const pair<double, Cliente>& a, const pair<double, Cliente>& b) {
            return a.first < b.first;
        });

    // Tomar los primeros k
    size_t limite = min(static_cast<size_t>(k), candidatos.size());
    vector<Cliente> rcl;
    for (size_t i = 0; i < limite; ++i) {
        rcl.push_back(candidatos[i].second);
    }

    return rcl;
}

Cliente GRASP::elegirClienteRandom(const vector<Cliente>& lista) {
    if (lista.empty())
        return Cliente(depotId, 0); // Devuelve depósito si lista vacía (puedes cambiar si querés otro comportamiento)

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, lista.size() - 1);

    return lista[dis(gen)];
}