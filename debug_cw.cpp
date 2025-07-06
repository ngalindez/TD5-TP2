#include "src/VRPLIBReader.h"
#include "src/HeuristicaClarkeWright.h"
#include "src/Cliente.h"
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
        
        // Create clientes vector
        std::vector<Cliente> clientes;
        const auto& nodes = reader.getNodes();
        const auto& demands = reader.getDemands();
        
        for (const auto& node : nodes) {
            if (node.id != reader.getDepotId()) { // Skip depot
                clientes.emplace_back(node.id, demands[node.id]);
            }
        }
        
        std::cout << "\nNumber of clients: " << clientes.size() << std::endl;
        
        // Run Clarke & Wright
        HeuristicaClarkeWright cw(clientes, reader.getDistanceMatrix(), 
                                  reader.getCapacity(), reader.getDepotId(), 
                                  reader.getNumVehicles());
        Solucion sol = cw.resolver();
        
        std::cout << "\nClarke & Wright Solution:" << std::endl;
        std::cout << "Total cost: " << std::fixed << std::setprecision(6) << sol.getCostoTotal() << std::endl;
        std::cout << "Number of routes: " << sol.getRutas().size() << std::endl;
        
        const auto& rutas = sol.getRutas();
        for (size_t i = 0; i < rutas.size(); ++i) {
            const auto& ruta = rutas[i];
            std::cout << "\nRoute " << (i+1) << ":" << std::endl;
            std::cout << "  Cost: " << std::fixed << std::setprecision(6) << ruta.getCosto() << std::endl;
            std::cout << "  Demand: " << ruta.getDemandaActual() << std::endl;
            std::cout << "  Clients: ";
            const auto& clientes_ruta = ruta.getClientes();
            for (size_t j = 0; j < clientes_ruta.size(); ++j) {
                if (j > 0) std::cout << " -> ";
                std::cout << clientes_ruta[j];
            }
            std::cout << std::endl;
            
            // Manual cost verification
            double manual_cost = 0;
            for (size_t j = 0; j < clientes_ruta.size() - 1; ++j) {
                manual_cost += reader.getDistanceMatrix()[clientes_ruta[j]][clientes_ruta[j+1]];
            }
            std::cout << "  Manual cost: " << std::fixed << std::setprecision(6) << manual_cost << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 