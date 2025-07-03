#include "CWwLocalSearch.h"
#include "Cliente.h"
#include "HeuristicaClarkeWright.h"
#include "HeuristicaInsercionCercana.h"
#include "OperadorRelocate.h"
#include "OperadorSwap.h"
#include "Ruta.h"
#include "Solucion.h"
#include "VRPLIBReader.h"
#include <iostream>
#include <string>

using namespace std;

void printRutas(const Solucion &solucion, int depotId,
                const string &label = "Rutas generadas:") {
  cout << "\n" << label << endl;
  int rutaNum = 1;
  for (const auto &ruta : solucion.getRutas()) {
    cout << "Ruta " << rutaNum++ << ": ";
    cout << depotId << " -> ";
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
  cout << "5. Salir" << endl;
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

  cout << "Costo total: " << solucion.getCostoTotal() << endl;
  cout << "Número de rutas: " << solucion.getRutas().size() << endl;
  printRutas(solucion, depotId, "Rutas de Clarke & Wright:");

  return solucion;
}

Solucion runNearestInsertion(const vector<Cliente> &clientes,
                             const vector<vector<double>> &dist_matrix,
                             int capacity, int depotId, int numVehicles) {
  cout << "\n=== Ejecutando Heurística de Inserción Más Cercana ===" << endl;
  HeuristicaInsercionCercana heuristica(clientes, dist_matrix, capacity,
                                        depotId, numVehicles);
  Solucion solucion = heuristica.resolver();

  cout << "Costo total: " << solucion.getCostoTotal() << endl;
  cout << "Número de rutas: " << solucion.getRutas().size() << endl;
  printRutas(solucion, depotId, "Rutas de Inserción Más Cercana:");

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

void applyBothOperators(const Solucion &originalSolucion, int depotId,
                        const string &heuristicName) {
  applySwapOperator(originalSolucion, depotId, heuristicName);
  applyRelocateOperator(originalSolucion, depotId, heuristicName);
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

    for (int i = 0; i < nodos.size(); i++) {
      if (nodos[i].id != depotId) {
        clientes.push_back(Cliente(nodos[i].id, demandas[nodos[i].id]));
      }
    }

    vector<vector<double>> dist_matrix = reader.getDistanceMatrix();

    int choice;
    do {
      printMenu();
      cin >> choice;
      cin.ignore(); // Limpiar el salto de línea del buffer de entrada

      switch (choice) {
      case 1: { // Clarke & Wright
        Solucion solucion =
            runClarkeWright(clientes, dist_matrix, reader.getCapacity(),
                            depotId, reader.getNumVehicles());

        int operatorChoice;
        printOperatorMenu();
        cin >> operatorChoice;
        cin.ignore();

        switch (operatorChoice) {
        case 1:
          applySwapOperator(solucion, depotId, "Clarke & Wright");
          break;
        case 2:
          applyRelocateOperator(solucion, depotId, "Clarke & Wright");
          break;
        case 3:
          applyBothOperators(solucion, depotId, "Clarke & Wright");
          break;
        case 4:
          cout << "No se aplicó búsqueda local." << endl;
          break;
        default:
          cout << "Opción inválida. No se aplicó búsqueda local." << endl;
        }
        break;
      }

      case 2: { // Inserción Más Cercana
        Solucion solucion =
            runNearestInsertion(clientes, dist_matrix, reader.getCapacity(),
                                depotId, reader.getNumVehicles());

        int operatorChoice;
        printOperatorMenu();
        cin >> operatorChoice;
        cin.ignore();

        switch (operatorChoice) {
        case 1:
          applySwapOperator(solucion, depotId, "Inserción Más Cercana");
          break;
        case 2:
          applyRelocateOperator(solucion, depotId, "Inserción Más Cercana");
          break;
        case 3:
          applyBothOperators(solucion, depotId, "Inserción Más Cercana");
          break;
        case 4:
          cout << "No se aplicó búsqueda local." << endl;
          break;
        default:
          cout << "Opción inválida. No se aplicó búsqueda local." << endl;
        }
        break;
      }

      case 3: { // Ambas Heurísticas
        Solucion solucionCW =
            runClarkeWright(clientes, dist_matrix, reader.getCapacity(),
                            depotId, reader.getNumVehicles());
        Solucion solucionNI =
            runNearestInsertion(clientes, dist_matrix, reader.getCapacity(),
                                depotId, reader.getNumVehicles());

        int operatorChoice;
        printOperatorMenu();
        cin >> operatorChoice;
        cin.ignore();

        switch (operatorChoice) {
        case 1:
          applySwapOperator(solucionCW, depotId, "Clarke & Wright");
          applySwapOperator(solucionNI, depotId, "Inserción Más Cercana");
          break;
        case 2:
          applyRelocateOperator(solucionCW, depotId, "Clarke & Wright");
          applyRelocateOperator(solucionNI, depotId, "Inserción Más Cercana");
          break;
        case 3:
          applyBothOperators(solucionCW, depotId, "Clarke & Wright");
          applyBothOperators(solucionNI, depotId, "Inserción Más Cercana");
          break;
        case 4:
          cout << "No se aplicó búsqueda local." << endl;
          break;
        default:
          cout << "Opción inválida. No se aplicó búsqueda local." << endl;
        }
        break;
      }

      case 4: { // Clarke & Wright + Local Search
        cout << "\n=== Clarke & Wright + Local Search (Swap + Relocate hasta "
                "mínimo local) ==="
             << endl;
        CWwLocalSearch cwwls(clientes, dist_matrix, reader.getCapacity(),
                             depotId, reader.getNumVehicles());
        Solucion solucion = cwwls.resolver();
        cout << "Costo total después de búsqueda local: "
             << solucion.getCostoTotal() << endl;
        cout << "Número de rutas: " << solucion.getRutas().size() << endl;
        printRutas(solucion, depotId,
                   "Rutas finales (Clarke & Wright + Local Search):");
        break;
      }

      case 5:
        cout << "Saliendo..." << endl;
        break;

      default:
        cout << "Opción inválida. Por favor intenta de nuevo." << endl;
      }
    } while (choice != 5);

  } catch (const exception &e) {
    cerr << "Error leyendo archivo: " << e.what() << endl;
    return 1;
  }

  return 0;
}