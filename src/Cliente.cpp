#include "Cliente.h"

Cliente::Cliente(int id, int demand) : id(id), demand(demand) {}

int Cliente::getId() const {
    return id;
}

int Cliente::getDemand() const {
    return demand;
}