#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include "OperadorSwap.h"
#include "OperadorRelocate.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class GRASP {
private:
    const vector<Cliente>& clientes;
    const vector<vector<double>>& distMatrix;
    const unordered_map<int, int>& id2pos;
    int capacidadVehiculo;
    int depotId;
    int numVehiculos;
    int numIter;
    int kRCL;

    Cliente elegirClienteRandom(const vector<Cliente>& lista);
    vector<Cliente> buscarKcercanos(int desde, const vector<bool>& visitado, int k);
    Solucion construirConRCL(int k);

public:
    GRASP(const vector<Cliente>& clientes,
          const vector<vector<double>>& distMatrix,
          const unordered_map<int, int>& id2pos,
          int capacidadVehiculo,
          int depotId,
          int numVehiculos,
          int numIter,
          int kRCL);

    // Construcción randomizada con RCL
    Solucion resolver();
};
