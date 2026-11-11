#include "item.h"

Door::Door(int x, int y)
{
    // do the math here.
}

void openDoor(Player &p)
{
    for (Item *i : p.getInventory())
    {
        i->use(p);
    }
}