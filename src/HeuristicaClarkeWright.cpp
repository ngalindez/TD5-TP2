#include "HeuristicaClarkeWright.h"
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include <algorithm> // Para std::find y std::reverse
#include <vector>

using namespace std;

HeuristicaClarkeWright::HeuristicaClarkeWright(
    const vector<Cliente> &clientes, const vector<vector<double>> &distMatrix,
    int capacidadVehiculo, int depotId, int numVehiculos)
    : clientes(clientes), distMatrix(distMatrix),
      capacidadVehiculo(capacidadVehiculo), depotId(depotId),
      numVehiculos(numVehiculos) {}

Solucion HeuristicaClarkeWright::resolver() {
  // 1. Inicializar rutas individuales
  vector<Ruta> rutas;
  inicializarRutas(rutas);
  Solucion sol(clientes, distMatrix, numVehiculos);

  // 2. Calcular ahorros
  vector<Ahorro> ahorros = calcularAhorros();

  // 3. Ordenar ahorros (ya se hace en calcularAhorros, pero por claridad lo
  // mencionamos acá) std::sort(ahorros.begin(), ahorros.end(),
  // std::greater<Ahorro>());

  // 4. Fusionar rutas según los ahorros y restricciones
  for (const auto &ahorro : ahorros) {
    int id1 = ahorro.cliente_i;
    int id2 = ahorro.cliente_j;

    // Buscar las rutas que contienen a id1 y id2
    int idx1 = -1, idx2 = -1;
    for (size_t i = 0; i < rutas.size(); ++i) {
      const auto &clientes = rutas[i].getClientes();
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
  for (const auto& ruta : rutas) {
    sol.agregarRuta(ruta);
  }

  return sol;
}

vector<HeuristicaClarkeWright::Ahorro>
HeuristicaClarkeWright::calcularAhorros() {
  vector<Ahorro> ahorros;

  // Calcular ahorros para cada par de clientes
  for (size_t i = 0; i < clientes.size(); i++) {
    if (clientes[i].getId() == depotId)
      continue;

    for (size_t j = i + 1; j < clientes.size(); j++) {
      if (clientes[j].getId() == depotId)
        continue;

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

void HeuristicaClarkeWright::inicializarRutas(vector<Ruta> &rutas) {
  rutas.clear();
  for (const auto &cliente : clientes) {
    if (cliente.getId() == depotId) continue;
    // Armamos la ruta de un solo cliente
    vector<int> soloCliente = {cliente.getId()};
    rutas.emplace_back(capacidadVehiculo, depotId, distMatrix,
                       clientes, soloCliente);
  }
}

bool HeuristicaClarkeWright::esFactibleFusion(const Ruta &r1, const Ruta &r2,
  int id1, int id2) {
// No deben estar en la misma ruta
if (&r1 == &r2) return false;

const auto& c1 = r1.getClientes();
const auto& c2 = r2.getClientes();

// Chequea si id1 está en la primera o última POSICIÓN INTERIOR
bool id1_extremo = (c1[1] == id1) || (c1[c1.size()-2] == id1);
bool id2_extremo = (c2[1] == id2) || (c2[c2.size()-2] == id2);

// Capacidad
int demandaTotal = r1.getDemandaActual() + r2.getDemandaActual();
bool capacidad_ok = demandaTotal <= capacidadVehiculo;

return id1_extremo && id2_extremo && capacidad_ok;
}

void HeuristicaClarkeWright::fusionarRutas(vector<Ruta> &rutas, int idx1,
  int idx2, int id1, int id2) {
Ruta &r1 = rutas[idx1];
Ruta &r2 = rutas[idx2];

vector<int> clientes1 = r1.getClientes();
vector<int> clientes2 = r2.getClientes();

// Quitar depósito de los extremos
vector<int> sub1(clientes1.begin()+1, clientes1.end()-1);
vector<int> sub2(clientes2.begin()+1, clientes2.end()-1);

vector<int> nuevaRuta;

// Determinar el orden correcto y si hay que invertir
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

// Crear la nueva ruta
Ruta nueva(capacidadVehiculo, depotId, distMatrix, clientes, nuevaRuta);

// Eliminar las rutas originales
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

