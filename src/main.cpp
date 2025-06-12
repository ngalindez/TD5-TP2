#include <iostream>
#include "VRPLIBReader.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << endl;
        return 1;
    }

    VRPLIBReader reader(argv[1]);

    cout << "Instance Name: " << reader.getName() << endl;
    cout << "Dimension: " << reader.getDimension() << endl;
    cout << "Number of Vehicles: " << reader.getNumVehicles() << endl;
    cout << "Capacity: " << reader.getCapacity() << endl;
    // Notar que el deposito varia y no siempre es 0. getDepotId() nos dice cual es el id del deposito.
    cout << "Depot ID: " << reader.getDepotId() << endl;

    vector<Node> clients = reader.getNodes();

    cout << clients.size() << endl;
    
    // Testeamos la matriz de distancia. Tomando los id de los nodos, indexa desde 1.
    // Tener en cuenta esto al momento de decidir como representar una solucion.
    cout << "Distance matrix" << endl;
    vector<vector<double>> dist_matrix = reader.getDistanceMatrix();
    for (int i = 1; i <= reader.getDimension(); i++) {
        for (int j = 1; j <= reader.getDimension(); j++) {
            cout << dist_matrix[i][j] << " ";
        }
        cout << endl;
    }

    

    return 0;
}