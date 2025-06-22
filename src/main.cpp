#include <iostream>
#include "VRPLIBReader.h"
#include "Cliente.h"
#include "Ruta.h"
#include "Solucion.h"
#include "HeuristicaClarkWright.h"
#include "HeuristicaInsercionCercana.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << endl;
        return 1;
    }

    VRPLIBReader reader(argv[1]);

    cout << "Instance Name: " << reader.getName() << endl;
    cout << "Dimension: " << reader.getDimension() << endl;
    cout << "Number of Vehicles: " << reader.getNumVehicles() << endl;
    cout << "Capacity: " << reader.getCapacity() << endl;

    // Inicialización de los clientes.
    vector<Node> nodos = reader.getNodes();
    vector<int> demandas = reader.getDemands();
    vector<Cliente> clientes;
    int depotId = reader.getDepotId();

    for (int i = 0; i < nodos.size(); i++)
    {
        if (nodos[i].id != depotId)
        {
            clientes.push_back(Cliente(nodos[i].id, demandas[nodos[i].id]));
        }
    }

    // Testeamos la matriz de distancia. Tomando los id de los nodos, indexa desde 1.
    // Tener en cuenta esto al momento de decidir como representar una solucion.
    vector<vector<double>> dist_matrix = reader.getDistanceMatrix();

    HeuristicaClarkWright heuristica(clientes, dist_matrix, reader.getCapacity(), depotId, reader.getNumVehicles());
    Solucion solucion = heuristica.resolver();

    // Mostrar resultados
    cout << "\nResultados de la Heurística de Clarke & Wright:" << endl;
    cout << "Costo total: " << solucion.getCostoTotal() << endl;
    cout << "Número de rutas: " << solucion.getRutas().size() << endl;
    
    // Mostrar cada ruta
    cout << "\nRutas generadas:" << endl;
    int rutaNum = 1;
    for (const auto& ruta : solucion.getRutas()) {
        cout << "Ruta " << rutaNum++ << ": ";
        cout << depotId << " -> ";
        for (size_t i = 0; i < ruta.getClientes().size(); ++i) {
            cout << ruta.getClientes()[i];
            if (i < ruta.getClientes().size() - 1) {
                cout << " -> ";
            }
}

        cout << " (Demanda: " << ruta.getDemandaActual() << ", Costo: " << ruta.getCosto() << ")" << endl;
    }


    cout << "\nResultados de la Heurística de Inserción Más Cercana:" << endl;

    HeuristicaInsercionCercana heuristicaInsercion(clientes, dist_matrix, reader.getCapacity(), depotId, reader.getNumVehicles());
    Solucion solucionInsercion = heuristicaInsercion.resolver();

    cout << "Costo total: " << solucionInsercion.getCostoTotal() << endl;
    cout << "Número de rutas: " << solucionInsercion.getRutas().size() << endl;

    cout << "\nRutas generadas:" << endl;
    int num = 1;
    for (const auto& ruta : solucionInsercion.getRutas()) {
        cout << "Ruta " << num++ << ": ";
        cout << reader.getDepotId() << " -> ";
        for (size_t i = 0; i < ruta.getClientes().size(); ++i) {
            cout << ruta.getClientes()[i];
            if (i < ruta.getClientes().size() - 1) {
                cout << " -> ";
            }
}
        cout << " (Demanda: " << ruta.getDemandaActual() << ", Costo: " << ruta.getCosto() << ")" << endl;
    }

        

    return 0;
}