#ifndef SOLUCION_H
#define SOLUCION_H

#include <vector>
#include "Ruta.h"
#include "Cliente.h"

using namespace std;

class Solucion
{
private:
    vector<Ruta> _rutas;
    double _costoTotal;
    int _cantCamiones;

public:
    Solucion(int cantCamiones);
    Solucion(const vector<Ruta>& rutas, const vector<Cliente>& clientes, 
             const vector<vector<double>>& distMatrix);
    bool agregarRuta(Ruta ruta);
    bool removerRuta(Ruta ruta);
    int getCostoTotal();
    int getCantCamiones();
    vector<Ruta> getRutas();
    // bool esFactible();
};

#endif