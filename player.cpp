#include <iostream>
#include "player.h"
using namespace std;

void Player::takeItem(Item *i)
{
    inventory.push_back(i);
}

vector<Item *> Player::getInventory()
{
    return inventory;
}

void Player::useItem(Room &r, Player &p)
{

    for (Item *i : r.getRoomItems())
    {
        vector<int> itemCoords = i->getCoords();
        if (coords[0] == itemCoords[0] && coords[1] == itemCoords[1])
        {
            if (i->getName() == "Generic Key")
            {
                r.removeItem(i);
                i->use(*i, p);
            }
            else
            {
                i->use(*i, p);
            }
        }
    }
}

Player::Player(int x, int y)
{

    if (x < 0 || y < 0 || x >= GRID || y >= GRID)
    {
        cout << "invalid start/end place for a Player" << endl;
        exit(0);
    }

    coords[0] = x;
    coords[1] = y;
}

vector<int> Player::getCoords()
{
    return coords;
}

vector<string> Player::getSprite()
{
    return sprite;
}

void Player::move(int x, int y)
{
    x = coords[0] + x;
    y = coords[1] + y;
    if (!(x < 0 || x >= GRID || y < 0 || y >= GRID))
    {
        coords[0] = x;
        coords[1] = y;
    }
}