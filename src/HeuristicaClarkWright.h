#ifndef HEURISTICA_CLARK_WRIGHT_H
#define HEURISTICA_CLARK_WRIGHT_H

#include <iostream>
#include <vector>
#include "VRPLIBReader.h"
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"

using namespace std;

class HeuristicaClarkWright {

    public:
        HeuristicaClarkWright(const vector<Cliente>& clientes,
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

        struct Ahorro {
            double valor;
            int cliente_i;
            int cliente_j;
            Ahorro(double v, int i, int j) : valor(v), cliente_i(i), cliente_j(j) {}
            bool operator>(const Ahorro& otro) const { return valor > otro.valor; }
        };

        vector<Ahorro> calcularAhorros();
        void inicializarRutas(vector<Ruta>& rutas);
        bool esFactibleFusion(const Ruta& r1, const Ruta& r2, int id1, int id2);
        void fusionarRutas(vector<Ruta>& rutas, int idx1, int idx2, int id1, int id2);
};

#endif