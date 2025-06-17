#include "Solucion.h"
#include <algorithm>

using namespace std;

Solucion::Solucion(int cantCamiones)
    : _cantCamiones(cantCamiones)
{
    _costoTotal = 0;
}

Solucion::Solucion(const vector<Ruta>& rutas, const vector<Cliente>& clientes, 
                   const vector<vector<double>>& distMatrix)
    : _rutas(rutas), _cantCamiones(rutas.size())
{
    _costoTotal = 0;
    for (const auto& ruta : rutas) {
        _costoTotal += ruta.getCosto();
    }
}

// Verificar que sea independiente (?)
bool Solucion::agregarRuta(Ruta ruta)
{
    if (_rutas.size() + 1 > _cantCamiones){
        return false;
    }

    _rutas.push_back(ruta);
    _costoTotal += ruta.getCosto();
    return true;
}

// MEJORAR, NO PASAR RUTA COMO PARÁMETRO
bool Solucion::removerRuta(Ruta ruta)
{
    auto it = find(_rutas.begin(), _rutas.end(), ruta);

    if (it == _rutas.end())
    {
        return false;
    }

    _costoTotal -= ruta.getCosto();
    _rutas.erase(it);
    return true;
}

int Solucion::getCostoTotal()
{
    return _costoTotal;
}

int Solucion::getCantCamiones()
{
    return _cantCamiones;
}

vector<Ruta> Solucion::getRutas()
{
    return _rutas;
}

// bool Solucion::esFactible()
// {
//     return _rutas.size() <= _cantCamiones;
// }