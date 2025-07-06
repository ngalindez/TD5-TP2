#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include <unordered_map>
#include <vector>

using namespace std;

class GRASP {
private:
    const vector<Cliente>& clientes;
    const vector<vector<double>>& distMatrix;
    const unordered_map<int, int>& id2pos;
    int capacidadVehiculo;
    int depotId;
    int numVehiculos;
    Cliente buscarMasCercanoRandom(int desde, const vector<bool>& visitado, int k);

public:
    GRASP(const vector<Cliente>& clientes,
                               const vector<vector<double>>& distMatrix,
                               const unordered_map<int, int>& id2pos,
                               int capacidadVehiculo,
                               int depotId,
                               int numVehiculos);

    // Construcción randomizada con RCL
    Solucion resolver(int k);

};