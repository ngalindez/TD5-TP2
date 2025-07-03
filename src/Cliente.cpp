#include "Cliente.h"
#include <stdexcept>

using namespace std;

Cliente::Cliente(int id, int demand) : id(id), demand(demand) {
    if (id < 0) {
        throw invalid_argument("El id no puede ser negativo");
    }
    if (demand < 0) {
        throw invalid_argument("La demanda no puede ser negativa");
    }
}

int Cliente::getId() const {
    return id;
}

int Cliente::getDemand() const {
    return demand;
}