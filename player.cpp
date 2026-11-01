#include "player.h"
#include "item.h"
using namespace std;

void Player::takeItem(Item *i)
{
    inventory.push_back(i);
}

Player::Player(int x, int y)
{

    if (x < 0 || y < 0 || x > 3 || y > 3)
    {
        cout << "invalid start/end place for a Player" << endl;
        exit(0);
    }

    coords[0] = (x * 18) + 6;
    coords[1] = ((y * 18) + 6) / 2;

    cout << "The Player's coords " << coords[0] << endl;
    cout << "The Player's coords " << coords[1] << endl;

    for (int c : coords)
    {
        cout << c << endl;
    }
}

vector<int> Player::getCoords()
{
    for (int dim : coords)
    {
        cout << "afters " << dim << endl;
    }
    return coords;
}

vector<string> Player::getSprite()
{
    return sprite;
}

void Player::useItem(Item *item, Room &room)
{
    item->use(*this, room);
}