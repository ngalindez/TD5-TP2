#include "CWwLocalSearch.h"

CWwLocalSearch::CWwLocalSearch(const vector<Cliente> &clientes,
                               const vector<vector<double>> &distMatrix,
                               int capacidadVehiculo, int depotId,
                               int numVehiculos)
    : clientes(clientes), distMatrix(distMatrix),
      capacidadVehiculo(capacidadVehiculo), depotId(depotId),
      numVehiculos(numVehiculos),
      heuristica(clientes, distMatrix, capacidadVehiculo, depotId,
                 numVehiculos) {}

Solucion CWwLocalSearch::resolver() {
  Solucion solucion = heuristica.resolver();
  bool improved = true;

  while (improved) {
    improved = false;
    // Aplicamos Swap
    OperadorSwap swapOp(solucion);
    Solucion swapSol = swapOp.aplicar();
    if (swapSol.getCostoTotal() < solucion.getCostoTotal()) {
      solucion = swapSol;
      improved = true;
    }

    // Aplicamos Relocate
    OperadorRelocate relocateOp(solucion);
    Solucion relocateSol = relocateOp.aplicar();
    if (relocateSol.getCostoTotal() < solucion.getCostoTotal()) {
      solucion = relocateSol;
      improved = true;
    }
  }
  return solucion;
}