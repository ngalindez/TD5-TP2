#ifndef RUTA_H
#define RUTA_H

#include <vector>
#include "Cliente.h"

using namespace std;

class Ruta {
private:
    vector<int> clientes;  // IDs de los clientes en la ruta
    int capacidadMaxima;
    int demandaActual;
    double costoTotal;
    int idDeposito;

public:
    Ruta(int capacidad, int deposito);
    
    // Getters
    const vector<int>& getClientes() const;
    int getCapacidadMaxima() const;
    int getDemandaActual() const;
    double getCostoTotal() const;
    
    // Métodos de modificación
    bool agregarCliente(int idCliente, int demanda, const vector<vector<double>>& distMatrix);
    bool removerCliente(int idCliente, int demanda, const vector<vector<double>>& distMatrix);
    void actualizarCosto(const vector<vector<double>>& distMatrix);
    
    // Validaciones
    bool esFactible() const;
    bool tieneCapacidadSuficiente(int demandaAdicional) const;
};

#endif 