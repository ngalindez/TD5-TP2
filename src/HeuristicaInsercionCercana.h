#include <vector>
#include <limits>
#include <stdexcept>
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"

using namespace std;

class HeuristicaInsercionCercana {

    public:
        HeuristicaInsercionCercana(const vector<Cliente>& clientes,
                                const vector<vector<double>>& distMatrix,
                                int capacidadVehiculo,
                                int depotId,
                                int numVehiculos);

        Solucion resolver();

    private:
        const vector<Cliente>& clientes;
        const vector<vector<double>>& distMatrix;
        int capacidadVehiculo;
        int depotId;
        int numVehiculos;

        Cliente buscarMasCercano(int desde, const vector<bool>& candidatos);
};