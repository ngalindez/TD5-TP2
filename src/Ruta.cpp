#include "Ruta.h"
#include "Cliente.h"
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

Ruta::Ruta(
  int capacidad,
  int deposito,
  const vector<vector<double>>& distMatrix,
  const vector<Cliente>& allClientes,
  const vector<int>& clientesIniciales
)
: capacidadMaxima(capacidad),
  demandaActual(0),
  costoTotal(0),
  idDeposito(deposito),
  distMatrix(&distMatrix),
  allClientes(&allClientes)
{
  // Si el vector inicial ya contiene el depósito, úsalo tal cual
  if (!clientesIniciales.empty() &&
      clientesIniciales.front() == deposito &&
      clientesIniciales.back() == deposito) {
    clientes = clientesIniciales;
  } else {
    // Si no, construilo agregando depósitos
    clientes.push_back(deposito);
    for (int cliente : clientesIniciales) {
      if (cliente != deposito) {
        clientes.push_back(cliente);
      }
    }
    clientes.push_back(deposito);
  }

  // Calcular demanda
  for (size_t i = 1; i < clientes.size()-1; ++i) {
    int id = clientes[i];
    for (const auto& c : allClientes) {
      if (c.getId() == id) {
        demandaActual += c.getDemand();
        break;
      }
    }
  }

  costoTotal = calcularCosto();
}


void Ruta::agregarCliente(int cliente) {
  if (cliente == -1) {
  // Ignorar IDs inválidos
  return;
  }


  // Encontrar la demanda del cliente
  int demanda = 0;
  for (const auto &c : *allClientes) {
    if (c.getId() == cliente) {
      demanda = c.getDemand();
      break;
    }
  }

  // Verificar si hay capacidad suficiente
  if (demandaActual + demanda <= capacidadMaxima) {
    // Insertar antes del último depósito
    clientes.insert(clientes.end() - 1, cliente);
    demandaActual += demanda;
    costoTotal = calcularCosto();
  }
}

void Ruta::eliminarCliente(int cliente) {
  if (clientes.size() <= 2) {
    // No hay clientes intermedios
    return;
  }

  // Buscar en el rango sin depósitos
  auto it = find(clientes.begin() + 1, clientes.end() - 1, cliente);
  if (it != clientes.end() - 1) {
    // Encontrar la demanda del cliente
    int demanda = 0;
    for (const auto &c : *allClientes) {
      if (c.getId() == cliente) {
        demanda = c.getDemand();
        break;
      }
    }

    clientes.erase(it);
    demandaActual -= demanda;
    costoTotal = calcularCosto();
  }
}

double Ruta::calcularCosto() {
  double costo = 0;
  for (size_t i = 0; i < clientes.size() - 1; i++) {
    double d = (*distMatrix)[clientes[i]][clientes[i+1]];
    costo += d;
  }
  return costo;
}

bool Ruta::esFactible() const { return demandaActual <= capacidadMaxima; }

bool Ruta::tieneCapacidadSuficiente(int demandaAdicional) const {
  return demandaActual + demandaAdicional <= capacidadMaxima;
}

int Ruta::getCantidadClientes() const {
  return clientes.size() - 2; // Excluir los depósitos
}

const vector<int> &Ruta::getClientes() const { return clientes; }

double Ruta::getCosto() const { return costoTotal; }

int Ruta::getCapacidadMaxima() const { return capacidadMaxima; }

int Ruta::getDemandaActual() const { return demandaActual; }

int Ruta::getIdDeposito() const { return idDeposito; }

const vector<vector<double>> *Ruta::getDistMatrix() const { return distMatrix; }

const vector<Cliente> *Ruta::getAllClientes() const { return allClientes; }
