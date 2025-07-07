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
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

void printRutas(const Solucion &solucion, int depotId,
                const string &label = "Rutas generadas:") {
  (void)depotId; // Mark as unused to avoid warning
  cout << "\n" << label << endl;
  int rutaNum = 1;
  for (const auto &ruta : solucion.getRutas()) {
    cout << "Ruta " << rutaNum++ << ": ";
    for (size_t i = 0; i < ruta.getClientes().size(); ++i) {
      cout << ruta.getClientes()[i];
      if (i < ruta.getClientes().size() - 1) {
        cout << " -> ";
      }
    }
    cout << " (Demanda: " << ruta.getDemandaActual()
         << ", Costo: " << ruta.getCosto() << ")" << endl;
  }
}

void printMenu() {
  cout << "\n=== Menú del Solver CVRP ===" << endl;
  cout << "1. Heurística de Clarke & Wright" << endl;
  cout << "2. Heurística de Inserción Más Cercana" << endl;
  cout << "3. Ambas Heurísticas" << endl;
  cout << "4. Clarke & Wright + Local Search (Swap + Relocate hasta mínimo "
          "local)"
       << endl;
  cout << "5. GRASP" << endl;
  cout << "6. Salir" << endl;
  cout << "Elige una opción: ";
}

void printOperatorMenu() {
  cout << "\n=== Menú de Operadores de Búsqueda Local ===" << endl;
  cout << "1. Operador Swap" << endl;
  cout << "2. Operador Relocate" << endl;
  cout << "3. Ambos Operadores" << endl;
  cout << "4. Sin Búsqueda Local" << endl;
  cout << "Elige una opción: ";
}

Solucion runClarkeWright(const vector<Cliente> &clientes,
                         const vector<vector<double>> &dist_matrix,
                         int capacity, int depotId, int numVehicles) {
  cout << "\n=== Ejecutando Heurística de Clarke & Wright ===" << endl;
  HeuristicaClarkeWright heuristica(clientes, dist_matrix, capacity, depotId,
                                    numVehicles);
  Solucion solucion = heuristica.resolver();
  
  if (solucion.esFactible()){
    cout << "Costo total: " << solucion.getCostoTotal() << endl;
    cout << "Número de rutas: " << solucion.getRutas().size() << endl;
    printRutas(solucion, depotId, "Rutas de Clarke & Wright:");

  } else {
    cout << "La heuristica de Clarke & Wright, devuelve una solucion invalida para esta instancia" << endl;
  }



  return solucion;
}

Solucion runGRASP(const vector<Cliente>& clientes,
                  const vector<vector<double>>& dist_matrix,
                  int capacity,
                  int depotId,
                  int numVehicles) {
    cout << "\n=== Ejecutando GRASP ===" << endl;
    
    cout << "Ingrese número de iteraciones para GRASP: ";
    int numIter;
    cin >> numIter;
    cin.ignore();

    cout << "Ingrese k para la RCL (ej. 3): ";
    int kRCL;
    cin >> kRCL;
    cin.ignore();

    unordered_map<int, int> id2pos;
    id2pos[depotId] = 0;
    for (size_t i = 0; i < clientes.size(); ++i) {
        id2pos[clientes[i].getId()] = i + 1;
    }

    GRASP grasp(clientes, dist_matrix, id2pos, capacity, depotId, numVehicles, numIter, kRCL);

    Solucion solucion = grasp.resolver();

    if (solucion.esFactible()){
      cout << "Costo total: " << solucion.getCostoTotal() << endl;
      cout << "Número de rutas: " << solucion.getRutas().size() << endl;
    printRutas(solucion, depotId, "Rutas de GRASP");

  } else {
    cout << "La metaheuristica GRASP, devuelve una solucion invalida para esta instancia" << endl;
  }

    return solucion;
}

Solucion runNearestInsertion(const vector<Cliente> &clientes,
                             const vector<vector<double>> &dist_matrix,
                             int capacity, int depotId, int numVehicles) {
  cout << "\n=== Ejecutando Heurística de Inserción Más Cercana ===" << endl;
  
  // Create id2pos mapping
  unordered_map<int, int> id2pos;
  id2pos[depotId] = 0;  // Depot is always at position 0
  for (size_t i = 0; i < clientes.size(); ++i) {
    id2pos[clientes[i].getId()] = i + 1;  // Clients start at position 1
  }
  
  HeuristicaInsercionCercana heuristica(clientes, dist_matrix, id2pos, capacity,
                                        depotId, numVehicles);
  Solucion solucion = heuristica.resolver();
  if(solucion.esFactible()){
    cout << "Costo total: " << solucion.getCostoTotal() << endl;
    cout << "Número de rutas: " << solucion.getRutas().size() << endl;
    printRutas(solucion, depotId, "Rutas de Inserción Más Cercana:");

  } else {
    cout << "La heuristica de Inserción Más Cercana, devuelve una solucion invalida para esta instancia" << endl;
  }

  return solucion;
}

void applySwapOperator(const Solucion &originalSolucion, int depotId,
                       const string &heuristicName) {
  cout << "\n=== Aplicando Operador Swap a " << heuristicName << " ===" << endl;
  cout << "Costo original: " << originalSolucion.getCostoTotal() << endl;

  OperadorSwap swapOperator(originalSolucion);
  Solucion solucionMejorada = swapOperator.aplicar();

  cout << "Costo después del swap: " << solucionMejorada.getCostoTotal()
       << endl;
  cout << "Mejora: "
       << originalSolucion.getCostoTotal() - solucionMejorada.getCostoTotal()
       << endl;
  printRutas(solucionMejorada, depotId, "Rutas mejoradas después del swap:");
}

void applyRelocateOperator(const Solucion &originalSolucion, int depotId,
                           const string &heuristicName) {
  cout << "\n=== Aplicando Operador Relocate a " << heuristicName
       << " ===" << endl;
  cout << "Costo original: " << originalSolucion.getCostoTotal() << endl;

  OperadorRelocate relocateOperator(originalSolucion);
  Solucion solucionMejorada = relocateOperator.aplicar();

  cout << "Costo después del relocate: " << solucionMejorada.getCostoTotal()
       << endl;
  cout << "Mejora: "
       << originalSolucion.getCostoTotal() - solucionMejorada.getCostoTotal()
       << endl;
  printRutas(solucionMejorada, depotId,
             "Rutas mejoradas después del relocate:");
}

void applyBothOperators(const Solucion &originalSolucion, int depotId, const string &heuristicName) {
  cout << "\n=== Aplicando ambos operadores (Swap + Relocate) a " << heuristicName << " ===" << endl;
  cout << "Costo original: " << originalSolucion.getCostoTotal() << endl;

  OperadorSwap swapOperator(originalSolucion);
  Solucion swapSol = swapOperator.aplicar();

  OperadorRelocate relocateOperator(swapSol);
  Solucion finalSol = relocateOperator.aplicar();

  cout << "Costo después de ambos operadores: " << finalSol.getCostoTotal() << endl;
  cout << "Mejora total: " << originalSolucion.getCostoTotal() - finalSol.getCostoTotal() << endl;
  printRutas(finalSol, depotId, "Rutas mejoradas después de ambos operadores:");
}

int main() {
  string filePath;
  cout << "Ingresa la ruta al archivo de instancia VRP: ";
  getline(cin, filePath);

  // Validar ruta del archivo
  if (filePath.empty()) {
    cerr << "Error: No se proporcionó ruta de archivo." << endl;
    return 1;
  }

  try {
    VRPLIBReader reader(filePath);

    cout << "\nNombre de la Instancia: " << reader.getName() << endl;
    cout << "Dimensión: " << reader.getDimension() << endl;
    cout << "Número de Vehículos: " << reader.getNumVehicles() << endl;
    cout << "Capacidad: " << reader.getCapacity() << endl;

    // Inicializar clientes
    vector<Node> nodos = reader.getNodes();
    vector<int> demandas = reader.getDemands();
    vector<Cliente> clientes;
    int depotId = reader.getDepotId();

    for (size_t i = 0; i < nodos.size(); ++i) {
      if (nodos[i].id != depotId) {
        clientes.push_back(Cliente(nodos[i].id, demandas[nodos[i].id]));
      }
    }

    vector<vector<double>> dist_matrix = reader.getDistanceMatrix();

    // Main menu: choose heuristic
    int heuristicChoice = 0;
    while (heuristicChoice != 1 && heuristicChoice != 2) {
      cout << "\n=== Menú Principal ===" << endl;
      cout << "1. Heurística de Clarke & Wright" << endl;
      cout << "2. Heurística de Inserción Más Cercana" << endl;
      cout << "Elige una opción: ";
      cin >> heuristicChoice;
      cin.ignore();
      if (heuristicChoice != 1 && heuristicChoice != 2) {
        cout << "Opción inválida. Intenta de nuevo." << endl;
      }
    }

    Solucion baseSolucion = (heuristicChoice == 1)
        ? runClarkeWright(clientes, dist_matrix, reader.getCapacity(), depotId, reader.getNumVehicles())
        : runNearestInsertion(clientes, dist_matrix, reader.getCapacity(), depotId, reader.getNumVehicles());
    string heuristicaNombre = (heuristicChoice == 1) ? "Clarke & Wright" : "Inserción Más Cercana";
    

    // Operator/Metaheuristic menu
    bool running = true;
    Solucion bestSol = baseSolucion;
    double bestCost = baseSolucion.getCostoTotal();
    int bestNumRutas = baseSolucion.getRutas().size();
    string bestLabel = "Solución base (" + heuristicaNombre + ")";
    while (running) {
      cout << "\n=== Opciones de Optimización ===" << endl;
      cout << "1. Aplicar Operador Swap" << endl;
      cout << "2. Aplicar Operador Relocate" << endl;
      cout << "3. Aplicar Ambos Operadores (Swap + Relocate)" << endl;
      cout << "4. Ejecutar GRASP" << endl;
      cout << "5. Salir y mostrar resumen" << endl;
      cout << "Elige una opción: ";
      int opChoice;
      cin >> opChoice;
      cin.ignore();
      switch (opChoice) {
        case 1: {
          OperadorSwap swapOperator(baseSolucion);
          Solucion swapSol = swapOperator.aplicar();
          applySwapOperator(baseSolucion, depotId, heuristicaNombre);
          if (swapSol.esFactible() && swapSol.getCostoTotal() < bestCost) {
            bestSol = swapSol;
            bestCost = swapSol.getCostoTotal();
            bestNumRutas = swapSol.getRutas().size();
            bestLabel = "Swap aplicado a " + heuristicaNombre;
          }
          break;
        }
        case 2: {
          OperadorRelocate relocateOperator(baseSolucion);
          Solucion relocateSol = relocateOperator.aplicar();
          applyRelocateOperator(baseSolucion, depotId, heuristicaNombre);
          if (relocateSol.esFactible() && relocateSol.getCostoTotal() < bestCost) {
            bestSol = relocateSol;
            bestCost = relocateSol.getCostoTotal();
            bestNumRutas = relocateSol.getRutas().size();
            bestLabel = "Relocate aplicado a " + heuristicaNombre;
          }
          break;
        }
        case 3: {
          OperadorSwap swapOperator(baseSolucion);
          Solucion swapSol = swapOperator.aplicar();
          OperadorRelocate relocateOperator(swapSol);
          Solucion finalSol = relocateOperator.aplicar();
          applyBothOperators(baseSolucion, depotId, heuristicaNombre);
          if (finalSol.esFactible() && finalSol.getCostoTotal() < bestCost) {
            bestSol = finalSol;
            bestCost = finalSol.getCostoTotal();
            bestNumRutas = finalSol.getRutas().size();
            bestLabel = "Swap + Relocate aplicados a " + heuristicaNombre;
          }
          break;
        }
        case 4: {
          Solucion graspSol = runGRASP(clientes, dist_matrix, reader.getCapacity(), depotId, reader.getNumVehicles());
          if (graspSol.esFactible() && (bestCost == 0 || graspSol.getCostoTotal() < bestCost)) {
            bestSol = graspSol;
            bestCost = graspSol.getCostoTotal();
            bestNumRutas = graspSol.getRutas().size();
            bestLabel = "GRASP";
          }
          break;
        }
        case 5: {
          running = false;
          break;
        }
        default:
          cout << "Opción inválida. Intenta de nuevo." << endl;
      }
    }

    // Print summary
    cout << "\n=== Resumen Final ===" << endl;
    cout << "Mejor solución encontrada: " << bestLabel << endl;
    cout << "Costo total: " << bestCost << endl;
    cout << "Número de rutas: " << bestNumRutas << endl;
    printRutas(bestSol, depotId, "Rutas de la mejor solución:");

  } catch (const exception &e) {
    cerr << "Error leyendo archivo: " << e.what() << endl;
    return 1;
  }

  return 0;
}