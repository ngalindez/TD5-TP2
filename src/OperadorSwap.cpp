#include "OperadorSwap.h"

OperadorSwap::OperadorSwap(const Solucion &solucion) : solucion(solucion) {}

Solucion OperadorSwap::aplicar() {
  Solucion mejorSolucion = solucion;
  int mejorCosto = solucion.getCostoTotal();

  for (size_t i = 0; i < solucion.getRutas().size(); i++) {
    for (size_t j = i + 1; j < solucion.getRutas().size(); j++) {
      Solucion nuevaSolucion = mejorSwapEntreRutas(i, j);
      if (nuevaSolucion.getCostoTotal() < mejorCosto) {
        mejorSolucion = nuevaSolucion;
        mejorCosto = nuevaSolucion.getCostoTotal();
      }
    }
  }
  return mejorSolucion;
}

Solucion OperadorSwap::mejorSwapEntreRutas(size_t i, size_t j) {
  const vector<vector<double>> &distMatrix = solucion.getDistMatrix();
  Ruta ruta_i = solucion.getRutas()[i];
  Ruta ruta_j = solucion.getRutas()[j];
  int costo_i = ruta_i.getCosto();
  int costo_j = ruta_j.getCosto();
  int size_i = ruta_i.getClientes().size();
  int size_j = ruta_j.getClientes().size();
  int demanda_ruta_i = ruta_i.getDemandaActual();
  int demanda_ruta_j = ruta_j.getDemandaActual();
  Ruta mejorRuta_i = ruta_i;
  Ruta mejorRuta_j = ruta_j;
  int mejorCosto = costo_i + costo_j;

  for (size_t pos_i = 1; pos_i < static_cast<size_t>(size_i - 1); pos_i++) {
    vector<int> clientes_i = ruta_i.getClientes();
    vector<int> clientes_j = ruta_j.getClientes();

    // Obtener el cliente de la ruta i
    int cliente_i = clientes_i[pos_i];

    // Obtener demandas individuales de clientes
    int demanda_cliente_i = 0;
    int demanda_cliente_j = 0;
    const vector<Cliente> &allClientes = *ruta_i.getAllClientes();
    for (const auto &cliente : allClientes) {
      if (cliente.getId() == cliente_i) {
        demanda_cliente_i = cliente.getDemand();
        break;
      }
    }

    for (size_t pos_j = 1; pos_j < static_cast<size_t>(size_j - 1); pos_j++) {
      // Obtener el cliente de la ruta j
      int cliente_j = clientes_j[pos_j];

      // Obtener demanda del cliente j
      for (const auto &cliente : allClientes) {
        if (cliente.getId() == cliente_j) {
          demanda_cliente_j = cliente.getDemand();
          break;
        }
      }

      // Calcular nuevos costos incrementalmente
      int nuevoCosto_i = costo_i -
                         distMatrix[clientes_i[pos_i - 1]][cliente_i] -
                         distMatrix[cliente_i][clientes_i[pos_i + 1]] +
                         distMatrix[clientes_i[pos_i - 1]][cliente_j] +
                         distMatrix[cliente_j][clientes_i[pos_i + 1]];

      int nuevoCosto_j = costo_j -
                         distMatrix[clientes_j[pos_j - 1]][cliente_j] -
                         distMatrix[cliente_j][clientes_j[pos_j + 1]] +
                         distMatrix[clientes_j[pos_j - 1]][cliente_i] +
                         distMatrix[cliente_i][clientes_j[pos_j + 1]];

      // Verificar restricciones de capacidad
      int nuevaDemanda_i =
          demanda_ruta_i - demanda_cliente_i + demanda_cliente_j;
      int nuevaDemanda_j =
          demanda_ruta_j - demanda_cliente_j + demanda_cliente_i;

      // Verificar factibilidad y mejora
      if (nuevaDemanda_i <= ruta_i.getCapacidadMaxima() &&
          nuevaDemanda_j <= ruta_j.getCapacidadMaxima() &&
          nuevoCosto_i + nuevoCosto_j < mejorCosto) {

        // Crear copias para la operación de swap
        vector<int> nuevos_clientes_i = clientes_i;
        vector<int> nuevos_clientes_j = clientes_j;

        // Realizar el swap
        nuevos_clientes_i[pos_i] = cliente_j;
        nuevos_clientes_j[pos_j] = cliente_i;

        // Crear nuevas rutas
        Ruta nuevaRuta_i =
            Ruta(ruta_i.getCapacidadMaxima(),
                 ruta_i.getIdDeposito(), distMatrix, *ruta_i.getAllClientes(), nuevos_clientes_i);
        Ruta nuevaRuta_j =
            Ruta(ruta_j.getCapacidadMaxima(),
                 ruta_j.getIdDeposito(), distMatrix, *ruta_j.getAllClientes(), nuevos_clientes_j);

        // Verificar si las nuevas rutas son factibles
        if (nuevaRuta_i.esFactible() && nuevaRuta_j.esFactible()) {
          mejorRuta_i = nuevaRuta_i;
          mejorRuta_j = nuevaRuta_j;
          mejorCosto = nuevoCosto_i + nuevoCosto_j;
        }
      }
    }
  }

  // Crear nueva solución con el mejor swap entre las rutas i y j
  vector<Ruta> nuevasRutas = solucion.getRutas();
  nuevasRutas[i] = mejorRuta_i;
  nuevasRutas[j] = mejorRuta_j;
  Solucion nuevaSolucion =
      Solucion(solucion.getClientes(), distMatrix, solucion.getCantCamiones(), nuevasRutas);

  return nuevaSolucion;
}