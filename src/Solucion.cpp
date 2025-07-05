#include "Solucion.h"
#include <algorithm>

using namespace std;

Solucion::Solucion(
  const vector<Cliente>& clientes,
  const vector<vector<double>>& distMatrix,
  const int cantCamiones,
  const vector<Ruta>& rutas)
  : _clientes(clientes),
    _distMatrix(distMatrix),
    _cantCamiones(cantCamiones),
    _rutas(rutas),
    _costoTotal(0) 
{
  for (const auto& ruta : _rutas) {
      _costoTotal += ruta.getCosto();
  }
}

bool Solucion::agregarRuta(const Ruta& ruta) {
  if (_rutas.size() >= _cantCamiones) return false;
  _rutas.push_back(ruta);
  _costoTotal += ruta.getCosto();
  return true;
}


// MEJORAR, NO PASAR RUTA COMO PARÁMETRO
bool Solucion::removerRuta(size_t index) {
  if (index >= _rutas.size()) return false;
  _costoTotal -= _rutas[index].getCosto();
  _rutas.erase(_rutas.begin() + index);
  return true;
}


int Solucion::getCostoTotal() const { return _costoTotal; }

int Solucion::getCantCamiones() const { return _cantCamiones; }

vector<Ruta> Solucion::getRutas() const { return _rutas; }

const vector<Cliente> &Solucion::getClientes() const { return _clientes; }

const vector<vector<double>> &Solucion::getDistMatrix() const {
  return _distMatrix;
}

bool Solucion::esFactible() const { return _rutas.size() <= _cantCamiones; }