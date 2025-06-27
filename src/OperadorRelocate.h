#include "Solucion.h"

class OperadorRelocate{
public:
  OperadorRelocate(const Solucion &solucion);
  Solucion aplicar();
  Solucion mejorRelocateEntreRutas(size_t ruta1, size_t ruta2);

private:
  const Solucion &solucion;
};