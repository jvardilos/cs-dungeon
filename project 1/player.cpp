#include "player.h"
#include "item.h"

void Player::takeItem(Item *i)
{
    inventory.push_back(i);
}

void Player::updateCoords(int x, int y)
{
    coords[0] = x;
    coords[1] = y;

    for (int dim : coords)
    {
        std::cout << "coords " << dim << std::endl;
    }
}

std::vector<int> Player::getCoords()
{
    for (int dim : coords)
    {
        std::cout << "afters " << dim << std::endl;
    }
    return coords;
}

void Player::renderSprite()
{
    for (string line : sprite)
    {
        cout << line;
    }
}

void Player::useItem(Item *item, Room &room)
{
    item->use(*this, room);
}