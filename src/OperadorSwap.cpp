#include "OperadorSwap.h"

OperadorSwap::OperadorSwap(const Solucion &solucion) : solucion(solucion) {}

Solucion OperadorSwap::aplicar() {
  // Iterar sobre todos los posibles pares de clientes de rutas diferentes,
  // aplicar el swap y calcular el costo de la nueva solución luego elegir el
  // swap que minimice el costo

  Solucion mejorSolucion = solucion;
  int mejorCosto = solucion.getCostoTotal();

  // iteramos sobre todos los pares de rutas diferentes y obtenemos el mejor
  // swap entre esas rutas luego elegimos el swap que minimice el costo de entre
  // los swaps obtenidos
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

// MEJORAR PARA NO CREAR TANTAS RUTAS

Solucion OperadorSwap::mejorSwapEntreRutas(size_t i, size_t j) {
  // Iterar sobre todos los posibles pares de clientes de las rutas, aplicar el
  // swap y calcular el costo de la nueva solución luego elegir el swap que
  // minimice el costo
  Ruta ruta_i = solucion.getRutas()[i];
  Ruta ruta_j = solucion.getRutas()[j];
  int mejorCosto = ruta_i.getCosto() + ruta_j.getCosto();
  int size_i = ruta_i.getClientes().size();
  int size_j = ruta_j.getClientes().size();

  for (size_t pos_i = 1; pos_i < size_i - 1; pos_i++) {
    vector<int> clientes_i = ruta_i.getClientes();
    for (size_t pos_j = 1; pos_j < size_j - 1; pos_j++) {
      vector<int> clientes_j = ruta_j.getClientes();
      swap(clientes_i[pos_i], clientes_j[pos_j]);

      // construyo las nuevas rutas con los clientes intercambiados
      Ruta nuevaRuta_i =
          Ruta(clientes_i, ruta_i.getCapacidadMaxima(), ruta_i.getIdDeposito(),
               *ruta_i.getDistMatrix(), *ruta_i.getAllClientes());
      Ruta nuevaRuta_j =
          Ruta(clientes_j, ruta_j.getCapacidadMaxima(), ruta_j.getIdDeposito(),
               *ruta_j.getDistMatrix(), *ruta_j.getAllClientes());

      // Check if both routes are feasible after the swap
      if (nuevaRuta_i.esFactible() && nuevaRuta_j.esFactible() &&
          nuevaRuta_i.getCosto() + nuevaRuta_j.getCosto() < mejorCosto) {
        ruta_i = nuevaRuta_i;
        ruta_j = nuevaRuta_j;
        mejorCosto = nuevaRuta_i.getCosto() + nuevaRuta_j.getCosto();
      }
    }
  }

  // armo una nueva solución con el mejor swap entre las rutas i y j
  vector<Ruta> nuevasRutas = solucion.getRutas();
  nuevasRutas[i] = ruta_i;
  nuevasRutas[j] = ruta_j;
  Solucion nuevaSolucion =
      Solucion(nuevasRutas, solucion.getClientes(), solucion.getDistMatrix());

  return nuevaSolucion;
}