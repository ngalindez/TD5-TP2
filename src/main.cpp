#include "CWwLocalSearch.h"
#include "Cliente.h"
#include "HeuristicaClarkeWright.h"
#include "HeuristicaInsercionCercana.h"
#include "GRASP.h"
#include "OperadorRelocate.h"
#include "OperadorSwap.h"
#include "Ruta.h"
#include "Solucion.h"
#include "VRPLIBReader.h"

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;
using Clock = chrono::high_resolution_clock;

// Imprime las rutas solo si el usuario lo desea
void printRoutesIfDesired(const Solucion &sol) {
    cout << "¿Mostrar rutas completas? (s/n): ";
    char resp; 
    cin >> resp;
    if (resp == 's' || resp == 'S') {
        int idx = 1;
        for (const auto &r : sol.getRutas()) {
            cout << "  Ruta " << (idx++) << ": ";
            for (int id : r.getClientes()) {
                cout << id << " ";
            }
            cout << "(Dem: " << r.getDemandaActual()
                 << ", Costo: " << r.getCosto() << ")\n";
        }
    }
}

// Corre Clarke & Wright y devuelve la solución, imprimiendo costo, rutas y tiempo
Solucion runClarkeWright(const vector<Cliente>& clientes,
                         const vector<vector<double>>& dist,
                         int cap, int depot, int numVeh) {
    auto t0 = Clock::now();
    HeuristicaClarkeWright hw(clientes, dist, cap, depot, numVeh);
    Solucion sol = hw.resolver();
    double dt = chrono::duration<double>(Clock::now() - t0).count();
    if(sol.esFactible() && sol.vistoTodos()){
      cout << "[Clarke&Wright] Costo=" << sol.getCostoTotal()
          << "  Rutas=" << sol.getRutas().size()
          << "  Tiempo=" << dt << "s\n";
    }
    else{
      cout << "La heuristica de Clarke & Wright no encontro una solución factible. Por favor intentar con otra heuristica." << endl;
    }
    return sol;
}

// Corre Inserción Más Cercana
Solucion runNearestInsertion(const vector<Cliente>& clientes,
                             const vector<vector<double>>& dist,
                             int cap, int depot, int numVeh) {
    auto t0 = Clock::now();
    unordered_map<int,int> id2pos;
    id2pos[depot] = 0;
    for (size_t i = 0; i < clientes.size(); ++i)
        id2pos[clientes[i].getId()] = i+1;

    HeuristicaInsercionCercana hic(clientes, dist, id2pos, cap, depot, numVeh);
    Solucion sol = hic.resolver();
    double dt = chrono::duration<double>(Clock::now() - t0).count();
    if(sol.esFactible() && sol.vistoTodos()){
      cout << "[Inserción Cercana] Costo=" << sol.getCostoTotal()
         << "  Rutas=" << sol.getRutas().size()
         << "  Tiempo=" << dt << "s\n";
    }
    else{
      cout << "La heuristica de insercion cercana no encontro una solución factible. Por favor intentar con otra heuristica." << endl;
    }
    return sol;
}

// Corre GRASP
Solucion runGRASP(const vector<Cliente>& clientes,
                  const vector<vector<double>>& dist,
                  int cap, int depot, int numVeh) {
    cout << "Ingrese número de iteraciones GRASP: "; int it; cin >> it;
    cout << "Ingrese tamaño RCL (k): ";             int k;  cin >> k;
    unordered_map<int,int> id2pos;
    id2pos[depot] = 0;
    for (size_t i = 0; i < clientes.size(); ++i)
        id2pos[clientes[i].getId()] = i+1;

    auto t0 = Clock::now();
    GRASP g(clientes, dist, id2pos, cap, depot, numVeh, it, k);
    Solucion sol = g.resolver();
    double dt = chrono::duration<double>(Clock::now() - t0).count();
    if(sol.esFactible() && sol.vistoTodos()){
      cout << "[GRASP] Costo=" << sol.getCostoTotal()
           << "  Rutas=" << sol.getRutas().size()
           << "  Tiempo=" << dt << "s\n";
    }
    else{
      cout << "La meta-heuristica no encontro una solución factible. Por favor intentar con otra heuristica." << endl;
    }
    return sol;
}

// Menú de operadores de búsqueda local
void localSearchMenu(const Solucion &baseSol, const string &nombre) {
    double origCost = baseSol.getCostoTotal();
    bool salir = false;
    while (!salir) {
        cout << "\n=== Búsqueda Local para " << nombre << " ===\n"
             << "1. Operador Swap\n"
             << "2. Operador Relocate\n"
             << "3. Ambos operadores\n"
             << "4. Volver al menú principal\n"
             << "Seleccione: ";
        int opc; cin >> opc;
        switch (opc) {
          case 1: {
            auto t0 = Clock::now();
            OperadorSwap op(baseSol);
            Solucion s = op.aplicar();
            double dt = chrono::duration<double>(Clock::now() - t0).count();
            cout << "[Swap] Costo inicial=" << origCost
                 << "  Costo final=" << s.getCostoTotal()
                 << "  Δ=" << (origCost - s.getCostoTotal())
                 << "  Tiempo=" << dt << "s\n";
            printRoutesIfDesired(s);
            break;
          }
          case 2: {
            auto t0 = Clock::now();
            OperadorRelocate op(baseSol);
            Solucion s = op.aplicar();
            double dt = chrono::duration<double>(Clock::now() - t0).count();
            cout << "[Relocate] Costo inicial=" << origCost
                 << "  Costo final=" << s.getCostoTotal()
                 << "  Δ=" << (origCost - s.getCostoTotal())
                 << "  Tiempo=" << dt << "s\n";
            printRoutesIfDesired(s);
            break;
          }
          case 3: {
            auto t0 = Clock::now();
            OperadorSwap   os(baseSol);
            Solucion s1 = os.aplicar();
            OperadorRelocate orc(s1);
            Solucion s2 = orc.aplicar();
            double dt = chrono::duration<double>(Clock::now() - t0).count();
            cout << "[Swap+Reloc] Costo inicial=" << origCost
                 << "  Costo final=" << s2.getCostoTotal()
                 << "  Δ=" << (origCost - s2.getCostoTotal())
                 << "  Tiempo=" << dt << "s\n";
            printRoutesIfDesired(s2);
            break;
          }
          case 4:
            salir = true;
            break;
          default:
            cout << "Opción inválida.\n";
        }
    }
}

int main(int argc, char* argv[]) {
  string path;
  if (argc > 1) {
      path = argv[1];
      cout << "Usando archivo de instancia: " << path << endl;
  } else {
      cout << "Ingrese ruta al archivo VRP: ";
      getline(cin, path);
      if (path.empty()) return 1;
  }

    try {
      VRPLIBReader reader(path);

      // Construir vector<Cliente>
      vector<Cliente> clientes;
      for (auto &n : reader.getNodes()) {
        if (n.id != reader.getDepotId())
          clientes.emplace_back(n.id, reader.getDemands()[n.id]);
      }

      auto dist    = reader.getDistanceMatrix();
      int cap      = reader.getCapacity();
      int depot    = reader.getDepotId();
      int numVehic = reader.getNumVehicles();

      bool terminar = false;
      while (!terminar) {
        cout << "\n=== Menú Principal ===\n"
             << "1. Clarke & Wright\n"
             << "2. Inserción Más Cercana\n"
             << "3. GRASP\n"
             << "4. Salir\n"
             << "Seleccione: ";
        int h; cin >> h;
        cin.ignore();

        if (h == 4) {
          terminar = true;
          continue;
        }

        Solucion sol = (h==1)
          ? runClarkeWright     (clientes, dist, cap, depot, numVehic)
          : (h==2)
          ? runNearestInsertion (clientes, dist, cap, depot, numVehic)
          : /* h==3 */
            runGRASP            (clientes, dist, cap, depot, numVehic);

        // Only proceed if the solution is feasible and all clients are visited
        if (sol.esFactible() && sol.vistoTodos()) {
            printRoutesIfDesired(sol);
            string name = (h==1 ? "Clarke&Wright"
                         : h==2 ? "Inserción Cercana"
                                : "GRASP");
            localSearchMenu(sol, name);
        } else {
        }
      }

    } catch (const exception &e) {
      cerr << "Error: " << e.what() << "\n";
      return 1;
    }
    return 0;
}