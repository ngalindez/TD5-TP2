using namespace std;

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