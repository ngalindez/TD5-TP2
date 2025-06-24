#include "Ruta.h"
#include "Cliente.h"
#include <algorithm>
#include <vector>


using namespace std;

// Constructor básico
Ruta::Ruta(int capacidad, int deposito,
           const vector<vector<double>> &distMatrix,
           const vector<Cliente> &allClientes)
    : capacidadMaxima(capacidad), demandaActual(0), costoTotal(0),
      idDeposito(deposito), distMatrix(&distMatrix), allClientes(&allClientes) {
  // Inicializar con el depósito
  clientes.push_back(deposito);
}

// Constructor con clientes iniciales
Ruta::Ruta(const vector<int> &clientesIniciales, int capacidad, int deposito,
           const vector<vector<double>> &distMatrix,
           const vector<Cliente> &allClientes)
    : capacidadMaxima(capacidad), demandaActual(0), costoTotal(0),
      idDeposito(deposito), distMatrix(&distMatrix), allClientes(&allClientes) {
  // Agregar el depósito al inicio
  clientes.push_back(deposito);

  // Agregar los clientes iniciales
  for (int cliente : clientesIniciales) {
    if (cliente != deposito) {
      agregarCliente(cliente);
    }
  }

  // Agregar el depósito al final
  clientes.push_back(deposito);

  // Calcular el costo inicial
  costoTotal = calcularCosto();
}

void Ruta::agregarCliente(int cliente) {
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
    costo += (*distMatrix)[clientes[i]][clientes[i + 1]];
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
