#include "HeuristicaClarkeWright.h"
#include "OperadorRelocate.h"
#include "OperadorSwap.h"
#include <vector>

class CWwLocalSearch {
public:
  CWwLocalSearch(const vector<Cliente> &clientes,
                 const vector<vector<double>> &distMatrix,
                 int capacidadVehiculo, int depotId, int numVehiculos);
  Solucion resolver();

private:
  const vector<Cliente> &clientes;
  const vector<vector<double>> &distMatrix;
  int capacidadVehiculo;
  int depotId;
  int numVehiculos;

  HeuristicaClarkeWright heuristica;
};