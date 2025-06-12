#include <vector>
#include "Ruta.h"

using namespace std;

class Solucion
{
private:
    vector<Ruta> _rutas;
    double _costoTotal;
    int _cantCamiones;

public:
    Solucion(int cantCamiones);
    bool agregarRuta(Ruta ruta);
    bool removerRuta(Ruta ruta);
    int getCostoTotal();
    int getCantCamiones();
    vector<Ruta> getRutas();
    // bool esFactible();
};