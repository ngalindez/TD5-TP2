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
    vector<int> soloCliente = {cliente.getId()};
    rutas.emplace_back(soloCliente, capacidadVehiculo, depotId, distMatrix,
                       clientes);
  }
}

bool HeuristicaClarkeWright::esFactibleFusion(const Ruta &r1, const Ruta &r2,
                                              int id1, int id2) {
  // No deben estar en la misma ruta
  if (&r1 == &r2)
    return false;
  // Deben estar en los extremos de sus rutas
  bool id1_extremo =
      (r1.getClientes().front() == id1) || (r1.getClientes().back() == id1);
  bool id2_extremo =
      (r2.getClientes().front() == id2) || (r2.getClientes().back() == id2);
  // Capacidad
  int demandaTotal = r1.getDemandaActual() + r2.getDemandaActual();
  bool capacidad_ok = demandaTotal <= capacidadVehiculo;
  return id1_extremo && id2_extremo && capacidad_ok;
}

void HeuristicaClarkeWright::fusionarRutas(vector<Ruta> &rutas, int idx1,
                                           int idx2, int id1, int id2) {
  // Referencias a las rutas que vamos a fusionar
  Ruta &r1 = rutas[idx1];
  Ruta &r2 = rutas[idx2];

 
  vector<int> clientes1 = r1.getClientes();
  vector<int> clientes2 = r2.getClientes();
  vector<int> nuevaRuta;


  // Determinar el orden correcto y si hay que invertir alguna ruta
  if (clientes1.back() == id1 && clientes2.front() == id2) {
    // Caso 1: r1 termina en id1, r2 empieza en id2 → unir directo: r1 + r2
    nuevaRuta = clientes1;
    nuevaRuta.insert(nuevaRuta.end(), clientes2.begin(), clientes2.end());
  } else if (clientes1.front() == id1 && clientes2.back() == id2) {
    // Caso 2: r1 empieza en id1, r2 termina en id2 → unir: r2 + r1
    nuevaRuta = clientes2;
    nuevaRuta.insert(nuevaRuta.end(), clientes1.begin(), clientes1.end());
  } else if (clientes1.back() == id1 && clientes2.back() == id2) {
    // Caso 3: r1 termina en id1, r2 termina en id2 → invertir r2 antes de unir: r1 + reverse(r2)
    reverse(clientes2.begin(), clientes2.end());
    nuevaRuta = clientes1;
    nuevaRuta.insert(nuevaRuta.end(), clientes2.begin(), clientes2.end());
  } else if (clientes1.front() == id1 && clientes2.front() == id2) {
    // Caso 4: r1 empieza en id1, r2 empieza en id2 → invertir r1 antes de unir: reverse(r1) + r2
    reverse(clientes1.begin(), clientes1.end());
    nuevaRuta = clientes1;
    nuevaRuta.insert(nuevaRuta.end(), clientes2.begin(), clientes2.end());
  }

  // Crear la nueva ruta fusionada
  // El constructor de Ruta automáticamente añade el depósito al principio y al final
  Ruta nueva(nuevaRuta, capacidadVehiculo, depotId, distMatrix, clientes);

  // Eliminar rutas viejas del vector
  // Se elimina primero el índice mayor para no desacomodar los índices
  if (idx1 > idx2) {
    rutas.erase(rutas.begin() + idx1);
    rutas.erase(rutas.begin() + idx2);
  } else {
    rutas.erase(rutas.begin() + idx2);
    rutas.erase(rutas.begin() + idx1);
  }

  // Agregar la nueva ruta fusionada al vector de rutas
  rutas.push_back(nueva);
}
