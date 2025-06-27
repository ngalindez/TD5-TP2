#include "OperadorRelocate.h"
#include "Solucion.h"

OperadorRelocate::OperadorRelocate(const Solucion &solucion)
    : solucion(solucion) {}

Solucion OperadorRelocate::aplicar() {
  Solucion mejorSolucion = solucion;
  int mejorCosto = solucion.getCostoTotal();

  for (size_t i = 0; i < solucion.getRutas().size(); i++) {
    for (size_t j = i + 1; j < solucion.getRutas().size(); j++) {
      Solucion nuevaSolucion = mejorRelocateEntreRutas(i, j);
      if (nuevaSolucion.getCostoTotal() < mejorCosto) {
        mejorSolucion = nuevaSolucion;
        mejorCosto = nuevaSolucion.getCostoTotal();
      }
    }
  }
  return mejorSolucion;
}

// VER QUE NO SE PISEN LAS RUTAS
Solucion OperadorRelocate::mejorRelocateEntreRutas(size_t i, size_t j) {
  const vector<vector<double>> &distMatrix = solucion.getDistMatrix();
  Ruta ruta_j = solucion.getRutas()[j];
  Ruta ruta_i = solucion.getRutas()[i];
  int costo_i = ruta_i.getCosto();
  int costo_j = ruta_j.getCosto();
  int size_i = ruta_i.getClientes().size();
  int size_j = ruta_j.getClientes().size();
  int demanda_ruta_i = ruta_i.getDemandaActual();
  int demanda_ruta_j = ruta_j.getDemandaActual();
  Ruta mejorRuta_i = ruta_i;
  Ruta mejorRuta_j = ruta_j;
  int mejorCosto = costo_i + costo_j;

  for (size_t pos_i = 1; pos_i < size_i - 1; pos_i++) {
    vector<int> clientes_i = ruta_i.getClientes();
    vector<int> clientes_j = ruta_j.getClientes();

    // Obtener el cliente a mover
    int cliente_a_mover = clientes_i[pos_i];

    // Obtener demanda individual del cliente
    int demanda_cliente = 0;
    const vector<Cliente> &allClientes = *ruta_i.getAllClientes();
    for (const auto &cliente : allClientes) {
      if (cliente.getId() == cliente_a_mover) {
        demanda_cliente = cliente.getDemand();
        break;
      }
    }

    for (size_t pos_j = 1; pos_j < size_j; pos_j++) {
      // Calcular nuevos costos (simplificado - podrías querer recalcular
      // apropiadamente)
      int nuevoCosto_i =
          costo_i - distMatrix[clientes_i[pos_i - 1]][clientes_i[pos_i]] -
          distMatrix[clientes_i[pos_i]][clientes_i[pos_i + 1]] +
          distMatrix[clientes_i[pos_i - 1]][clientes_i[pos_i + 1]];

      int nuevoCosto_j = costo_j -
                         distMatrix[clientes_j[pos_j - 1]][clientes_j[pos_j]] +
                         distMatrix[clientes_j[pos_j - 1]][cliente_a_mover] +
                         distMatrix[cliente_a_mover][clientes_j[pos_j]];

      int nuevaDemanda_j = demanda_cliente + demanda_ruta_j;

      // Verificar factibilidad y mejora
      if (nuevaDemanda_j <= ruta_j.getCapacidadMaxima() &&
          (demanda_ruta_i - demanda_cliente) <= ruta_i.getCapacidadMaxima() &&
          nuevoCosto_i + nuevoCosto_j < mejorCosto) {

        // Crear copias para la operación de relocate
        vector<int> nuevos_clientes_i = clientes_i;
        vector<int> nuevos_clientes_j = clientes_j;

        // Remover cliente de la ruta i
        nuevos_clientes_i.erase(nuevos_clientes_i.begin() + pos_i);

        // Insertar cliente en la ruta j
        nuevos_clientes_j.insert(nuevos_clientes_j.begin() + pos_j,
                                 cliente_a_mover);

        // Crear nuevas rutas
        Ruta nuevaRuta_i =
            Ruta(nuevos_clientes_i, ruta_i.getCapacidadMaxima(),
                 ruta_i.getIdDeposito(), distMatrix, *ruta_i.getAllClientes());
        Ruta nuevaRuta_j =
            Ruta(nuevos_clientes_j, ruta_j.getCapacidadMaxima(),
                 ruta_j.getIdDeposito(), distMatrix, *ruta_j.getAllClientes());

        // Verificar si las nuevas rutas son factibles
        if (nuevaRuta_i.esFactible() && nuevaRuta_j.esFactible()) {
          mejorRuta_i = nuevaRuta_i;
          mejorRuta_j = nuevaRuta_j;
          mejorCosto = nuevoCosto_i + nuevoCosto_j;
        }
      }
    }
  }

  // Crear nueva solución con el mejor relocate entre las rutas i y j
  vector<Ruta> nuevasRutas = solucion.getRutas();
  nuevasRutas[i] = mejorRuta_i;
  nuevasRutas[j] = mejorRuta_j;
  Solucion nuevaSolucion =
      Solucion(nuevasRutas, solucion.getClientes(), distMatrix);

  return nuevaSolucion;
}