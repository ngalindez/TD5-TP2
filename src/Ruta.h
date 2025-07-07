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
    const vector<vector<double>>* distMatrix;
    const vector<Cliente>* allClientes;

public:
    // Constructor básico
    Ruta(int capacidad, int deposito, const vector<vector<double>>& distMatrix, const vector<Cliente>& allClientes, const vector<int> &clientesIniciales = {});
    
    // Métodos de modificación
    void agregarCliente(int cliente);
    void eliminarCliente(int cliente);
    
    // Métodos de consulta
    double calcularCosto();
    bool esFactible() const;
    bool tieneCapacidadSuficiente(int demandaAdicional) const;
    int getCantidadClientes() const;
    const vector<int>& getClientes() const;
    double getCosto() const;
    int getCapacidadMaxima() const;
    int getDemandaActual() const;
    int getIdDeposito() const;
    const vector<vector<double>>* getDistMatrix() const;
    const vector<Cliente>* getAllClientes() const;
    
    // Operador de igualdad
    bool operator==(const Ruta& otra) const {
        return clientes == otra.clientes && 
               capacidadMaxima == otra.capacidadMaxima && 
               idDeposito == otra.idDeposito;
    }
};

#endif 