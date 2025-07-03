#ifndef OPERADOR_RELOCATE_H
#define OPERADOR_RELOCATE_H
#include "Solucion.h"

class OperadorRelocate {
public:
  OperadorRelocate(const Solucion &solucion);
  Solucion aplicar();

private:
  const Solucion &solucion;
  Solucion mejorRelocateEntreRutas(size_t ruta1, size_t ruta2);
};

#endif // OPERADOR_RELOCATE_H