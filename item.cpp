#include "item.h"

Door::Door(string name, int x, int y) : Item(name, x, y) {}
Key::Key(string name, int x, int y) : Item(name, x, y) {}

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
string Item::getName()
{
    return name;
}

void Door::use(Item &roomItem, Player &p)
{
    for (Item *i : p.getInventory())
    {
        if (roomItem.getName() == "North Door" && i->getName() == "Generic Key")
        {
            islocked = false;
            cout << name << " has been unlocked with " << i->getName() << endl;
        }
    }
}

void Key::use(Item &roomItem, Player &p)
{
    if (name == "Generic Key")
    {
        p.takeItem(&roomItem);

        cout << roomItem.getName() << " was picked up and added to player's inventory" << endl;
    }
}

vector<string> Door::getSprite()
{
    return opened;
}

vector<string> Key::getSprite()
{
    return {sprite};
}