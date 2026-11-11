#include <iostream>
#include "player.h"
// #include "item.h"
using namespace std;

void Player::takeItem(Item *i)
{
    inventory.push_back(i);
}

vector<Item *> Player::getInventory()
{
    return inventory;
}

void Player::useItem(Room &)
{
    // if in section with item
    // if inventory item
    // pick up item
    // else
    // use item
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

bool Player::move(int dx, int dy)
{
    int nx = coords[0] + dx, ny = coords[1] + dy;
    if (nx < 0 || nx >= GRID || ny < 0 || ny >= GRID)
        return false; // wall; later, the hook for walking through an exit
    coords[0] = nx;
    coords[1] = ny;
    return true;
}