#include <iostream>
#include <vector>
#include <algorithm>  // For std::find and std::reverse
#include "VRPLIBReader.h"
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include "HeuristicaClarkWright.h"

using namespace std;

HeuristicaClarkWright::HeuristicaClarkWright(const vector<Cliente>& clientes,
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

Solucion HeuristicaClarkWright::resolver() {
    // 1. Inicializar rutas individuales
    vector<Ruta> rutas;
    inicializarRutas(rutas);

    // 2. Calcular ahorros
    vector<Ahorro> ahorros = calcularAhorros();

    // 3. Ordenar ahorros (ya se hace en calcularAhorros, pero por claridad lo mencionamos aquí)
    // std::sort(ahorros.begin(), ahorros.end(), std::greater<Ahorro>());

    // 4. Fusionar rutas según los ahorros y restricciones
    for (const auto& ahorro : ahorros) {
        int id1 = ahorro.cliente_i;
        int id2 = ahorro.cliente_j;

        // Buscar las rutas que contienen a id1 y id2
        int idx1 = -1, idx2 = -1;
        for (size_t i = 0; i < rutas.size(); ++i) {
            const auto& clientes = rutas[i].getClientes();
            if (std::find(clientes.begin(), clientes.end(), id1) != clientes.end()) {
                idx1 = i;
            }
            if (std::find(clientes.begin(), clientes.end(), id2) != clientes.end()) {
                idx2 = i;
            }
        }

        // Si ambos clientes están en rutas diferentes y la fusión es factible
        if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
            if (esFactibleFusion(rutas[idx1], rutas[idx2], id1, id2)) {
                fusionarRutas(rutas, idx1, idx2, id1, id2);
            }
        }
    }

    // 5. Devolver la solución final
    return Solucion(rutas, clientes, distMatrix);
}

vector<HeuristicaClarkWright::Ahorro> HeuristicaClarkWright::calcularAhorros() {
    vector<Ahorro> ahorros;
    
    // Calcular ahorros para cada par de clientes
    for (size_t i = 0; i < clientes.size(); i++) {
        if (clientes[i].getId() == depotId) continue;
        
        for (size_t j = i + 1; j < clientes.size(); j++) {
            if (clientes[j].getId() == depotId) continue;
            
            // Calcular el ahorro: costo(depot,i) + costo(j,depot) - costo(i,j)
            double ahorro = distMatrix[depotId][clientes[i].getId()] +
                           distMatrix[clientes[j].getId()][depotId] -
                           distMatrix[clientes[i].getId()][clientes[j].getId()];
            
            ahorros.emplace_back(ahorro, clientes[i].getId(), clientes[j].getId());
        }
    }
    
    // Ordenar ahorros de mayor a menor
    sort(ahorros.begin(), ahorros.end(), greater<Ahorro>());
    
    return ahorros;
}

void HeuristicaClarkWright::inicializarRutas(vector<Ruta>& rutas){
    rutas.clear();
    for (const auto& cliente : clientes) {
        vector<int> soloCliente = {cliente.getId()};
        rutas.emplace_back(soloCliente, capacidadVehiculo, depotId, distMatrix, clientes);
    }
}

bool HeuristicaClarkWright::esFactibleFusion(const Ruta& r1, const Ruta& r2, int id1, int id2){
    // No deben estar en la misma ruta
    if (&r1 == &r2) return false;
    // Deben estar en los extremos de sus rutas
    bool id1_extremo = (r1.getClientes().front() == id1) || (r1.getClientes().back() == id1);
    bool id2_extremo = (r2.getClientes().front() == id2) || (r2.getClientes().back() == id2);
    // Capacidad
    int demandaTotal = r1.getDemandaActual() + r2.getDemandaActual();
    bool capacidad_ok = demandaTotal <= capacidadVehiculo;
    return id1_extremo && id2_extremo && capacidad_ok;
}

void HeuristicaClarkWright::fusionarRutas(vector<Ruta>& rutas, int idx1, int idx2, int id1, int id2){
    // Fusiona rutas[idx1] y rutas[idx2] según los extremos id1 y id2
    Ruta& r1 = rutas[idx1];
    Ruta& r2 = rutas[idx2];
    vector<int> nuevaRuta;

    // Determinar el orden correcto y si hay que invertir alguna ruta
    if (r1.getClientes().back() == id1 && r2.getClientes().front() == id2) {
        // r1 + r2
        nuevaRuta = r1.getClientes();
        nuevaRuta.insert(nuevaRuta.end(), r2.getClientes().begin(), r2.getClientes().end());
    } else if (r1.getClientes().front() == id1 && r2.getClientes().back() == id2) {
        // r2 + r1
        nuevaRuta = r2.getClientes();
        nuevaRuta.insert(nuevaRuta.end(), r1.getClientes().begin(), r1.getClientes().end());
    } else if (r1.getClientes().back() == id1 && r2.getClientes().back() == id2) {
        // r1 + reverse(r2)
        vector<int> r2rev = r2.getClientes();
        std::reverse(r2rev.begin(), r2rev.end());
        nuevaRuta = r1.getClientes();
        nuevaRuta.insert(nuevaRuta.end(), r2rev.begin(), r2rev.end());
    } else if (r1.getClientes().front() == id1 && r2.getClientes().front() == id2) {
        // reverse(r1) + r2
        vector<int> r1rev = r1.getClientes();
        std::reverse(r1rev.begin(), r1rev.end());
        nuevaRuta = r1rev;
        nuevaRuta.insert(nuevaRuta.end(), r2.getClientes().begin(), r2.getClientes().end());
    }

    // Crear la nueva ruta fusionada
    Ruta nueva(nuevaRuta, capacidadVehiculo, depotId, distMatrix, clientes);

    // Eliminar las rutas viejas (eliminar primero el índice mayor para no invalidar el menor)
    if (idx1 > idx2) {
        rutas.erase(rutas.begin() + idx1);
        rutas.erase(rutas.begin() + idx2);
    } else {
        rutas.erase(rutas.begin() + idx2);
        rutas.erase(rutas.begin() + idx1);
    }
    // Agregar la nueva ruta
    rutas.push_back(nueva);
}

                            