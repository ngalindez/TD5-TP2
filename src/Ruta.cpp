#include "Ruta.h"

using namespace std; 

Ruta::Ruta(int capacidad, int deposito) 
    : capacidadMaxima(capacidad), demandaActual(0), costoTotal(0), idDeposito(deposito) {}

const vector<int>& Ruta::getClientes() const {
    return clientes;
}

int Ruta::getCapacidadMaxima() const {
    return capacidadMaxima;
}

int Ruta::getDemandaActual() const {
    return demandaActual;
}

double Ruta::getCostoTotal() const {
    return costoTotal;
}

bool Ruta::agregarCliente(int idCliente, int demanda, const vector<vector<double>>& distMatrix) {
    if (!tieneCapacidadSuficiente(demanda)) {
        return false;
    }
    
    clientes.push_back(idCliente);
    demandaActual += demanda;
    actualizarCosto(distMatrix);
    return true;
}

bool Ruta::removerCliente(int idCliente, int demanda, const vector<vector<double>>& distMatrix) {
    auto it = find(clientes.begin(), clientes.end(), idCliente);
    if (it == clientes.end()) {
        return false;
    }
    
    // Guardamos la posición del cliente a remover
    int pos = it - clientes.begin();
    
    // Actualizamos el costo antes de remover el cliente
    if (clientes.size() == 1) {
        // Si es el único cliente, simplemente restamos el costo ida y vuelta al depósito
        costoTotal -= (distMatrix[idDeposito][idCliente] + distMatrix[idCliente][idDeposito]);
    } else if (pos == 0) {
        // Si es el primer cliente
        costoTotal -= (distMatrix[idDeposito][idCliente] + distMatrix[idCliente][clientes[1]]);
        costoTotal += distMatrix[idDeposito][clientes[1]];
    } else if (pos == clientes.size() - 1) {
        // Si es el último cliente
        costoTotal -= (distMatrix[clientes[pos-1]][idCliente] + distMatrix[idCliente][idDeposito]);
        costoTotal += distMatrix[clientes[pos-1]][idDeposito];
    } else {
        // Si está en medio
        costoTotal -= (distMatrix[clientes[pos-1]][idCliente] + distMatrix[idCliente][clientes[pos+1]]);
        costoTotal += distMatrix[clientes[pos-1]][clientes[pos+1]];
    }
    
    clientes.erase(it);
    demandaActual -= demanda;
    return true;
}

void Ruta::actualizarCosto(const vector<vector<double>>& distMatrix) {
    if (clientes.empty()) {
        costoTotal = 0;
        return;
    }
    
    if (clientes.size() == 1) {
        // Si solo hay un cliente, el costo es ida y vuelta al depósito
        costoTotal = distMatrix[idDeposito][clientes[0]] + distMatrix[clientes[0]][idDeposito];
        return;
    }
    
    // Para más de un cliente, actualizamos el costo considerando el último cliente agregado
    int ultimoCliente = clientes.back();
    int penultimoCliente = clientes[clientes.size() - 2];
    
    // // Costo desde el depósito al primer cliente
    // if (!clientes.empty()) {
    //     costoTotal += distMatrix[idDeposito][clientes[0]];
    // }
    
    // // Costo entre clientes consecutivos
    // for (size_t i = 0; i < clientes.size() - 1; i++) {
    //     costoTotal += distMatrix[clientes[i]][clientes[i + 1]];
    // }
    
    // // Costo desde el último cliente al depósito
    // if (!clientes.empty()) {
    //     costoTotal += distMatrix[clientes.back()][idDeposito];
    // }
}

bool Ruta::esFactible() const {
    return demandaActual <= capacidadMaxima;
}

bool Ruta::tieneCapacidadSuficiente(int demandaAdicional) const {
    return (demandaActual + demandaAdicional) <= capacidadMaxima;
} 