#include "item.h"

Door::Door(int x, int y)
{
    x = coords[0] + x;
    y = coords[1] + y;
    if (!(x < 0 || x >= GRID || y < 0 || y >= GRID))
    {
        coords[0] = x;
        coords[1] = y;
    }
}

void openDoor(Player &p)
{
    for (Item *i : p.getInventory())
    {
        i->use(p);
    }
}