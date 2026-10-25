#ifndef ITEM
#define ITEM
#include <iostream>
using namespace std;

class Player;

struct Item
{
    virtual ~Item() = default;
    virtual void use(Player &, Room &) = 0;
};

struct Brazier : public Item
{
    bool isLit = false;
};

struct Torch : public Item
{
    virtual void use(Player &p, Room &r)
    {
        vector<int> playerXY = p.getCoords();
        std::cout << "use unimplemented for Torch";
    }
};

#endif