#include <chrono>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../src/GRASP.h"
#include "../src/HeuristicaClarkeWright.h"
#include "../src/HeuristicaInsercionCercana.h"
#include "../src/OperadorRelocate.h"
#include "../src/OperadorSwap.h"
#include "../src/VRPLIBReader.h"

using namespace std;

void print_result(const std::string &instance, int capacity, int total_demand,
                  const std::string &heuristic, const std::string &local_search,
                  double cost, int num_routes, double time,
                  const std::string &status, const std::string &msg = "") {
  std::cout << "instance:" << instance << std::endl;
  std::cout << "capacity:" << capacity << std::endl;
  std::cout << "total_demand:" << total_demand << std::endl;
  std::cout << "heuristic:" << heuristic << std::endl;
  std::cout << "local_search:" << local_search << std::endl;
  std::cout << "cost:" << cost << std::endl;
  std::cout << "num_routes:" << num_routes << std::endl;
  std::cout << "time:" << time << std::endl;
  std::cout << "status:" << status << std::endl;
  if (!msg.empty())
    std::cout << "msg:" << msg << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0]
              << " <instance_path> <heuristic> <local_search> [grasp_iters] "
                 "[grasp_kRCL]"
              << std::endl;
    return 2;
  }
  std::string instance_path = argv[1];
  std::string heuristic = argv[2]; // "cw", "ni", "grasp"
  std::string local_search =
      argv[3]; // "none", "swap", "relocate", "both" (ignored for grasp)
  int grasp_iters = 0, grasp_kRCL = 0;
  if (heuristic == "grasp") {
    if (argc < 6) {
      std::cerr
          << "For GRASP, provide [grasp_iters] [grasp_kRCL] as extra arguments."
          << std::endl;
      return 2;
    }
    grasp_iters = std::stoi(argv[4]);
    grasp_kRCL = std::stoi(argv[5]);
  }

  try {
    auto start = std::chrono::high_resolution_clock::now();
    VRPLIBReader reader(instance_path);
    std::string instance_name = reader.getName();
    int capacity = reader.getCapacity();
    int num_vehicles = reader.getNumVehicles();
    int depotId = reader.getDepotId();
    const std::vector<Node> &nodes = reader.getNodes();
    const std::vector<int> &demands = reader.getDemands();
    std::vector<Cliente> clientes;
    int total_demand = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
      if (nodes[i].id != depotId) {
        clientes.push_back(Cliente(nodes[i].id, demands[nodes[i].id]));
        total_demand += demands[nodes[i].id];
      }
    }
    const std::vector<std::vector<double>> &dist_matrix =
        reader.getDistanceMatrix();
    Solucion solucion(clientes, dist_matrix, num_vehicles);
    std::string status = "ok";
    std::string msg = "";
    double cost = -1;
    int num_routes = -1;
    // --- Heuristic selection ---
    if (heuristic == "cw") {
      HeuristicaClarkeWright heur(clientes, dist_matrix, capacity, depotId,
                                  num_vehicles);
      solucion = heur.resolver();
    } else if (heuristic == "ni") {
      std::unordered_map<int, int> id2pos;
      id2pos[depotId] = 0;
      for (size_t i = 0; i < clientes.size(); ++i) {
        id2pos[clientes[i].getId()] = i + 1;
      }
      HeuristicaInsercionCercana heur(clientes, dist_matrix, id2pos, capacity,
                                      depotId, num_vehicles);
      solucion = heur.resolver();
    } else if (heuristic == "grasp") {
      std::unordered_map<int, int> id2pos;
      id2pos[depotId] = 0;
      for (size_t i = 0; i < clientes.size(); ++i) {
        id2pos[clientes[i].getId()] = i + 1;
      }
      GRASP grasp(clientes, dist_matrix, id2pos, capacity, depotId,
                  num_vehicles, grasp_iters, grasp_kRCL);
      solucion = grasp.resolver();
    } else {
      print_result(instance_path, capacity, total_demand, heuristic,
                   local_search, -1, -1, 0, "error", "Unknown heuristic");
      return 2;
    }
    // --- Local search selection (if not GRASP) ---
    if (heuristic != "grasp") {
      if (local_search == "swap") {
        OperadorSwap swapOp(solucion);
        Solucion swapSol = swapOp.aplicar();
        if (swapSol.esFactible() &&
            swapSol.getCostoTotal() < solucion.getCostoTotal()) {
          solucion = swapSol;
        }
      } else if (local_search == "relocate") {
        OperadorRelocate relocateOp(solucion);
        Solucion relocateSol = relocateOp.aplicar();
        if (relocateSol.esFactible() &&
            relocateSol.getCostoTotal() < solucion.getCostoTotal()) {
          solucion = relocateSol;
        }
      } else if (local_search == "both") {
        OperadorSwap swapOp(solucion);
        Solucion swapSol = swapOp.aplicar();
        OperadorRelocate relocateOp(swapSol);
        Solucion finalSol = relocateOp.aplicar();
        if (finalSol.esFactible() &&
            finalSol.getCostoTotal() < solucion.getCostoTotal()) {
          solucion = finalSol;
        }
      } else if (local_search == "none") {
        // Do nothing
      } else {
        print_result(instance_path, capacity, total_demand, heuristic,
                     local_search, -1, -1, 0, "error", "Unknown local_search");
        return 2;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    // --- Check solution ---
    if (!solucion.esFactible() || !solucion.vistoTodos()) {
      status = "infeasible";
      msg = "Solution infeasible or not all clients visited";
    }
    cost = solucion.getCostoTotal();
    num_routes = solucion.getRutas().size();
    print_result(instance_path, capacity, total_demand, heuristic, local_search,
                 cost, num_routes, elapsed.count(), status, msg);
    return (status == "ok") ? 0 : 1;
  } catch (const std::exception &e) {
    print_result(argv[1], -1, -1, argc > 2 ? argv[2] : "",
                 argc > 3 ? argv[3] : "", -1, -1, 0, "error", e.what());
    return 2;
  }
}
