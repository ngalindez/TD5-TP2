#include "src/VRPLIBReader.h"
#include <iostream>
#include <iomanip>

int main() {
    try {
        VRPLIBReader reader("instancias/2l-cvrp-0/E072-04f.dat");
        
        std::cout << "Instance: " << reader.getName() << std::endl;
        std::cout << "Dimension: " << reader.getDimension() << std::endl;
        std::cout << "Capacity: " << reader.getCapacity() << std::endl;
        std::cout << "Vehicles: " << reader.getNumVehicles() << std::endl;
        std::cout << "Depot ID: " << reader.getDepotId() << std::endl;
        
        const auto& nodes = reader.getNodes();
        const auto& demands = reader.getDemands();
        const auto& distMatrix = reader.getDistanceMatrix();
        
        std::cout << "\nFirst 10 nodes:" << std::endl;
        for (int i = 0; i < std::min(10, (int)nodes.size()); ++i) {
            std::cout << "Node " << nodes[i].id << ": (" << nodes[i].x << ", " << nodes[i].y 
                      << ") Demand: " << demands[nodes[i].id] << std::endl;
        }
        
        std::cout << "\nSample distances:" << std::endl;
        std::cout << "Distance 1->2: " << std::fixed << std::setprecision(6) << distMatrix[1][2] << std::endl;
        std::cout << "Distance 1->3: " << std::fixed << std::setprecision(6) << distMatrix[1][3] << std::endl;
        std::cout << "Distance 2->3: " << std::fixed << std::setprecision(6) << distMatrix[2][3] << std::endl;
        
        // Calculate some route costs manually
        std::cout << "\nManual route cost calculations:" << std::endl;
        
        // Route from optimal solution: 1 -> 20 -> 29 -> 30 -> 21 -> 22 -> 28 -> 27 -> 42 -> 43 -> 44 -> 46 -> 53 -> 45 -> 52 -> 48 -> 47 -> 50 -> 70 -> 51 -> 49 -> 25 -> 24 -> 26 -> 23 -> 1
        std::vector<int> optimal_route = {1, 20, 29, 30, 21, 22, 28, 27, 42, 43, 44, 46, 53, 45, 52, 48, 47, 50, 70, 51, 49, 25, 24, 26, 23, 1};
        double optimal_cost = 0;
        for (size_t i = 0; i < optimal_route.size() - 1; ++i) {
            optimal_cost += distMatrix[optimal_route[i]][optimal_route[i+1]];
        }
        std::cout << "Optimal route cost: " << std::fixed << std::setprecision(6) << optimal_cost << std::endl;
        
        // Calculate total demand for this route
        int total_demand = 0;
        for (size_t i = 1; i < optimal_route.size() - 1; ++i) {
            total_demand += demands[optimal_route[i]];
        }
        std::cout << "Total demand: " << total_demand << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 