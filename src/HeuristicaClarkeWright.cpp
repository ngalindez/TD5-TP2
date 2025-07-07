#include "HeuristicaClarkeWright.h"
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include <algorithm> // std::find, std::reverse
#include <vector>

using namespace std;

HeuristicaClarkeWright::HeuristicaClarkeWright(
    const vector<Cliente>& clientes,
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

Solucion HeuristicaClarkeWright::resolver() {
    // Build idDistMatrix
    int maxId = depotId;
    for (const auto& c : clientes)
        maxId = std::max(maxId, c.getId());

    vector<vector<double>> idDistMatrix(maxId + 1, vector<double>(maxId + 1, 0.0));

    // Copy distances
    for (const auto& c1 : clientes) {
        for (const auto& c2 : clientes) {
            idDistMatrix[c1.getId()][c2.getId()] = distMatrix[c1.getId()][c2.getId()];
        }
    }
    // Include depot distances
    for (const auto& c : clientes) {
        idDistMatrix[depotId][c.getId()] = distMatrix[depotId][c.getId()];
        idDistMatrix[c.getId()][depotId] = distMatrix[c.getId()][depotId];
    }
    idDistMatrix[depotId][depotId] = 0.0;

    // Initialize single-customer routes
    vector<Ruta> rutas;
    inicializarRutas(rutas, idDistMatrix);

    Solucion sol(clientes, idDistMatrix, numVehiculos);

    // Compute savings
    vector<Ahorro> ahorros = calcularAhorros(idDistMatrix);

    // Merge routes
    for (const auto& ahorro : ahorros) {
        int id1 = ahorro.cliente_i;
        int id2 = ahorro.cliente_j;

        int idx1 = -1, idx2 = -1;
        for (size_t i = 0; i < rutas.size(); ++i) {
            const auto& clientesRuta = rutas[i].getClientes();
            if (std::find(clientesRuta.begin(), clientesRuta.end(), id1) != clientesRuta.end())
                idx1 = i;
            if (std::find(clientesRuta.begin(), clientesRuta.end(), id2) != clientesRuta.end())
                idx2 = i;
        }

        if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
            if (esFactibleFusion(rutas[idx1], rutas[idx2], id1, id2)) {
                fusionarRutas(rutas, idx1, idx2, id1, id2, idDistMatrix);
            }
        }
    }

    // Add all routes to solution
    for (const auto& ruta : rutas)
        sol.agregarRuta(ruta);

    return sol;
}

vector<HeuristicaClarkeWright::Ahorro>
HeuristicaClarkeWright::calcularAhorros(const vector<vector<double>>& idDistMatrix) {
    vector<Ahorro> ahorros;

    for (size_t i = 0; i < clientes.size(); ++i) {
        if (clientes[i].getId() == depotId)
            continue;

        for (size_t j = i + 1; j < clientes.size(); ++j) {
            if (clientes[j].getId() == depotId)
                continue;

            double ahorro =
                idDistMatrix[depotId][clientes[i].getId()] +
                idDistMatrix[clientes[j].getId()][depotId] -
                idDistMatrix[clientes[i].getId()][clientes[j].getId()];

            ahorros.emplace_back(ahorro, clientes[i].getId(), clientes[j].getId());
        }
    }

    sort(ahorros.begin(), ahorros.end(), greater<Ahorro>());
    return ahorros;
}

void HeuristicaClarkeWright::inicializarRutas(
    vector<Ruta>& rutas,
    const vector<vector<double>>& idDistMatrix) {
    rutas.clear();
    for (const auto& cliente : clientes) {
        if (cliente.getId() == depotId)
            continue;
        vector<int> soloCliente = {cliente.getId()};
        rutas.emplace_back(capacidadVehiculo, depotId, idDistMatrix, clientes, soloCliente);
    }
}

bool HeuristicaClarkeWright::esFactibleFusion(
    const Ruta& r1,
    const Ruta& r2,
    int id1,
    int id2) {
    if (&r1 == &r2)
        return false;

    const auto& c1 = r1.getClientes();
    const auto& c2 = r2.getClientes();

    bool id1_extremo = (c1[1] == id1) || (c1[c1.size() - 2] == id1);
    bool id2_extremo = (c2[1] == id2) || (c2[c2.size() - 2] == id2);

    int demandaTotal = r1.getDemandaActual() + r2.getDemandaActual();
    bool capacidad_ok = demandaTotal <= capacidadVehiculo;

    return id1_extremo && id2_extremo && capacidad_ok;
}

void HeuristicaClarkeWright::fusionarRutas(
    vector<Ruta>& rutas,
    int idx1,
    int idx2,
    int id1,
    int id2,
    const vector<vector<double>>& idDistMatrix) {
    Ruta& r1 = rutas[idx1];
    Ruta& r2 = rutas[idx2];

    vector<int> clientes1 = r1.getClientes();
    vector<int> clientes2 = r2.getClientes();

    vector<int> sub1(clientes1.begin() + 1, clientes1.end() - 1);
    vector<int> sub2(clientes2.begin() + 1, clientes2.end() - 1);

    vector<int> nuevaRuta;

    if (sub1.back() == id1 && sub2.front() == id2) {
        nuevaRuta = sub1;
        nuevaRuta.insert(nuevaRuta.end(), sub2.begin(), sub2.end());
    } else if (sub1.front() == id1 && sub2.back() == id2) {
        nuevaRuta = sub2;
        nuevaRuta.insert(nuevaRuta.end(), sub1.begin(), sub1.end());
    } else if (sub1.back() == id1 && sub2.back() == id2) {
        reverse(sub2.begin(), sub2.end());
        nuevaRuta = sub1;
        nuevaRuta.insert(nuevaRuta.end(), sub2.begin(), sub2.end());
    } else if (sub1.front() == id1 && sub2.front() == id2) {
        reverse(sub1.begin(), sub1.end());
        nuevaRuta = sub1;
        nuevaRuta.insert(nuevaRuta.end(), sub2.begin(), sub2.end());
    }

    Ruta nueva(capacidadVehiculo, depotId, idDistMatrix, clientes, nuevaRuta);

    if (idx1 > idx2) {
        rutas.erase(rutas.begin() + idx1);
        rutas.erase(rutas.begin() + idx2);
    } else {
        rutas.erase(rutas.begin() + idx2);
        rutas.erase(rutas.begin() + idx1);
    }

    rutas.push_back(nueva);
}
