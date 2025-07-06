#ifndef SOLUCION_H
#define SOLUCION_H

#include "Cliente.h"
#include "Ruta.h"
#include <vector>
#include <unordered_set>

using namespace std;

class Solucion {
private:
  vector<Ruta> _rutas;
  double _costoTotal;
  int _cantCamiones;
  vector<Cliente> _clientes;
  vector<vector<double>> _distMatrix;

public:
  Solucion(
    const vector<Cliente>& clientes,
    const vector<vector<double>>& distMatrix,
    int cantCamiones,
    const vector<Ruta>& rutas = {}
  );

  bool agregarRuta(const Ruta& ruta);
  bool removerRuta(size_t index);
  double getCostoTotal() const;
  int getCantCamiones() const;
  vector<Ruta> getRutas() const;
  const vector<Cliente>& getClientes() const;
  const vector<vector<double>>& getDistMatrix() const;
  bool esFactible() const;
  bool vistoTodos() const;

};

#endif
