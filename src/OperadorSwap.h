#ifndef OPERADOR_SWAP_H
#define OPERADOR_SWAP_H
#include "Solucion.h"

class OperadorSwap {
public:
  OperadorSwap(const Solucion &solucion);
  Solucion aplicar();
  Solucion mejorSwapEntreRutas(size_t ruta1, size_t ruta2);

private:
  const Solucion &solucion;
};

#endif // OPERADOR_SWAP_H