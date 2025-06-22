#ifndef CLIENTE_H
#define CLIENTE_H

#include <vector>

class Cliente
{
private:
    int id;
    int demand;

    public:
        Cliente(int id, int demand);
        int getId() const;
        int getDemand() const;
};

#endif