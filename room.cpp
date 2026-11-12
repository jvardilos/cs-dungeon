#include <iostream>
#include "room.h"
#include "player.h"
using namespace std;

void Room::placeItem(Item *item)
{
    roomItems.push_back(item);
}

vector<Item *> Room::getRoomItems()
{
    return roomItems;
}

void Room::flipCurrentRoom()
{
    current = !current;
}

bool Room::isCurrentRoom()
{
    return current;
}

void Room::printRoom(Player *p)
{
    vector<string> playerSprite = p->getSprite();
    vector<int> coords = p->getCoords();

    // cell -> screen position of the sprite's top-left, centered in its cell
    int playerX = 2 + coords[0] * CELL_W + (CELL_W - 3) / 2;
    int playerY = 2 + coords[1] * CELL_H + (CELL_H - 3) / 2;

    for (int x = 1; x <= dimensions; x++)
        cout << "=";

    cout << endl;

    for (int y = 2; y <= dimensions / 2; y++)
    {
        cout << "|";

        for (int x = 2; x < dimensions; x++)
        {
            char cell = ' ';

            // Position of this room cell relative to the sprite.
            int spriteX = x - playerX;
            int spriteY = y - playerY;

            if (spriteY >= 0 &&
                spriteY < static_cast<int>(playerSprite.size()) &&
                spriteX >= 0 &&
                spriteX < static_cast<int>(playerSprite[spriteY].size()))
            {
                cell = playerSprite[spriteY][spriteX];
            }

            cout << cell;
        }

        cout << "|\n";
    }

    for (int x = 1; x <= dimensions; x++)
        cout << "=";

    cout << '\n';
}