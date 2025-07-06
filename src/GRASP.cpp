#include "GRASP.h"
#include <algorithm>
#include <random>

using namespace std;

GRASP::GRASP(
    const vector<Cliente>& clientes,
    const vector<vector<double>>& distMatrix,
    const unordered_map<int, int>& id2pos,
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

Solucion GRASP::resolver(int k) {
    vector<Ruta> rutas;
    vector<bool> visitado(clientes.size(), false);
    visitado[id2pos.at(depotId)] = true;

    size_t cantidadVisitados = 0;
    size_t cantidadClientes = clientes.size();

    while (cantidadVisitados < cantidadClientes) {
        Ruta ruta(capacidadVehiculo, depotId, distMatrix, clientes);
        int actual = depotId;
        int carga = 0;
        bool added = false;

        while (true) {
            Cliente elegido = buscarMasCercanoRandom(actual, visitado, k);
            if (elegido.getId() == depotId) break;

            int demanda = elegido.getDemand();
            if (carga + demanda <= capacidadVehiculo) {
                ruta.agregarCliente(elegido.getId());
                visitado.at(id2pos.at(elegido.getId())) = true;
                carga += demanda;
                actual = elegido.getId();
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

    Solucion sol(clientes, distMatrix, numVehiculos);
    for (const auto& ruta : rutas) {
        sol.agregarRuta(ruta);
    }

    return sol;
}

Cliente GRASP::buscarMasCercanoRandom(int desde, const vector<bool>& visitado, int k) {
    vector<pair<double, Cliente>> candidatos;

    for (const Cliente& c : clientes) {
        int pos = id2pos.at(c.getId());
        if (!visitado[pos] && c.getId() != depotId) {
            double dist = distMatrix.at(id2pos.at(desde)).at(pos);
            candidatos.emplace_back(dist, c);
        }
    }

    if (candidatos.empty())
        return Cliente(depotId, 0);

    sort(candidatos.begin(), candidatos.end());
    size_t limite = min(static_cast<size_t>(k), candidatos.size());
    vector<Cliente> rcl;
    for (size_t i = 0; i < limite; ++i) {
        rcl.push_back(candidatos[i].second);
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, rcl.size() - 1);

    return rcl[dis(gen)];
}
