#include "Solucion.h"

using namespace std;

Solucion::Solucion(int cantCamiones)
    : _cantCamiones(cantCamiones)
{
    vector<Ruta> _rutas;
    _costoTotal = 0;
}

// Verificar que sea independiente (?)
bool Solucion::agregarRuta(Ruta ruta)
{
    if (_rutas.size() + 1 > _cantCamiones){
        return false;
    }

    _rutas.push_back(ruta);
    _costoTotal += ruta.getCostoTotal();
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

    _costoTotal -= ruta.getCostoTotal();
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