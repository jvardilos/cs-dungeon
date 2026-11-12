#include "item.h"

Door::Door(int x, int y) : Item(x, y) {}

void Item::updateCoords(int x, int y)
{
    x = coords[0] + x;
    y = coords[1] + y;
    if (!(x < 0 || x >= GRID || y < 0 || y >= GRID))
    {
        coords[0] = x;
        coords[1] = y;
    }
}

vector<int> Item::getCoords()
{
    return coords;
}

void Door::use(Player &p)
{
    for (Item *i : p.getInventory())
    {
        i->use(p);
    }
}
