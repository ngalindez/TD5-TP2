#ifndef SOLUCION_H
#define SOLUCION_H

#include "Cliente.h"
#include "Ruta.h"
#include <vector>

using namespace std;

class Solucion {
private:
  vector<Ruta> _rutas;
  double _costoTotal;
  int _cantCamiones;
  vector<Cliente> _clientes;
  vector<vector<double>> _distMatrix;


public:
  Solucion(int cantCamiones);
  Solucion(const vector<Ruta> &rutas, const vector<Cliente> &clientes,
           const vector<vector<double>> &distMatrix, const int cantCamiones);
  Solucion(const vector<Cliente> &clientes, const vector<vector<double>> &distMatrix, const int cantCamiones);
  bool agregarRuta(Ruta ruta);
  bool removerRuta(Ruta ruta);
  int getCostoTotal() const;
  int getCantCamiones() const;
  vector<Ruta> getRutas() const;
  const vector<Cliente> &getClientes() const;
  const vector<vector<double>> &getDistMatrix() const;
  bool esFactible() const;
};

#endif