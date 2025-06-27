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
  cout << "\n=== CVRP Solver Menu ===" << endl;
  cout << "1. Clarke & Wright Heuristic" << endl;
  cout << "2. Nearest Insertion Heuristic" << endl;
  cout << "3. Both Heuristics" << endl;
  cout << "4. Exit" << endl;
  cout << "Choose an option: ";
}

void printOperatorMenu() {
  cout << "\n=== Local Search Operator Menu ===" << endl;
  cout << "1. Swap Operator" << endl;
  cout << "2. Relocate Operator" << endl;
  cout << "3. Both Operators" << endl;
  cout << "4. No Local Search" << endl;
  cout << "Choose an option: ";
}

Solucion runClarkeWright(const vector<Cliente> &clientes,
                         const vector<vector<double>> &dist_matrix,
                         int capacity, int depotId, int numVehicles) {
  cout << "\n=== Running Clarke & Wright Heuristic ===" << endl;
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
  cout << "\n=== Running Nearest Insertion Heuristic ===" << endl;
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
  cout << "\n=== Applying Swap Operator to " << heuristicName << " ===" << endl;
  cout << "Original cost: " << originalSolucion.getCostoTotal() << endl;

  OperadorSwap swapOperator(originalSolucion);
  Solucion solucionMejorada = swapOperator.aplicar();

  cout << "Cost after swap: " << solucionMejorada.getCostoTotal() << endl;
  cout << "Improvement: "
       << originalSolucion.getCostoTotal() - solucionMejorada.getCostoTotal()
       << endl;
  printRutas(solucionMejorada, depotId, "Improved routes after swap:");
}

void applyRelocateOperator(const Solucion &originalSolucion, int depotId,
                           const string &heuristicName) {
  cout << "\n=== Applying Relocate Operator to " << heuristicName
       << " ===" << endl;
  cout << "Original cost: " << originalSolucion.getCostoTotal() << endl;

  OperadorRelocate relocateOperator(originalSolucion);
  Solucion solucionMejorada = relocateOperator.aplicar();

  cout << "Cost after relocate: " << solucionMejorada.getCostoTotal() << endl;
  cout << "Improvement: "
       << originalSolucion.getCostoTotal() - solucionMejorada.getCostoTotal()
       << endl;
  printRutas(solucionMejorada, depotId, "Improved routes after relocate:");
}

void applyBothOperators(const Solucion &originalSolucion, int depotId,
                        const string &heuristicName) {
  applySwapOperator(originalSolucion, depotId, heuristicName);
  applyRelocateOperator(originalSolucion, depotId, heuristicName);
}

int main() {
  string filePath;
  cout << "Enter the path to the VRP instance file: ";
  getline(cin, filePath);

  // Validate file path
  if (filePath.empty()) {
    cerr << "Error: No file path provided." << endl;
    return 1;
  }

  try {
    VRPLIBReader reader(filePath);

    cout << "\nInstance Name: " << reader.getName() << endl;
    cout << "Dimension: " << reader.getDimension() << endl;
    cout << "Number of Vehicles: " << reader.getNumVehicles() << endl;
    cout << "Capacity: " << reader.getCapacity() << endl;

    // Initialize clients
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
      cin.ignore(); // Clear the newline from the input buffer

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
          cout << "No local search applied." << endl;
          break;
        default:
          cout << "Invalid option. No local search applied." << endl;
        }
        break;
      }

      case 2: { // Nearest Insertion
        Solucion solucion =
            runNearestInsertion(clientes, dist_matrix, reader.getCapacity(),
                                depotId, reader.getNumVehicles());

        int operatorChoice;
        printOperatorMenu();
        cin >> operatorChoice;
        cin.ignore();

        switch (operatorChoice) {
        case 1:
          applySwapOperator(solucion, depotId, "Nearest Insertion");
          break;
        case 2:
          applyRelocateOperator(solucion, depotId, "Nearest Insertion");
          break;
        case 3:
          applyBothOperators(solucion, depotId, "Nearest Insertion");
          break;
        case 4:
          cout << "No local search applied." << endl;
          break;
        default:
          cout << "Invalid option. No local search applied." << endl;
        }
        break;
      }

      case 3: { // Both Heuristics
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
          applySwapOperator(solucionNI, depotId, "Nearest Insertion");
          break;
        case 2:
          applyRelocateOperator(solucionCW, depotId, "Clarke & Wright");
          applyRelocateOperator(solucionNI, depotId, "Nearest Insertion");
          break;
        case 3:
          applyBothOperators(solucionCW, depotId, "Clarke & Wright");
          applyBothOperators(solucionNI, depotId, "Nearest Insertion");
          break;
        case 4:
          cout << "No local search applied." << endl;
          break;
        default:
          cout << "Invalid option. No local search applied." << endl;
        }
        break;
      }

      case 4:
        cout << "Exiting..." << endl;
        break;

      default:
        cout << "Invalid option. Please try again." << endl;
      }
    } while (choice != 4);

  } catch (const exception &e) {
    cerr << "Error reading file: " << e.what() << endl;
    return 1;
  }

  return 0;
}