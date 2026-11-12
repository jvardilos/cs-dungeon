#include <iostream>
#include <algorithm>
#include "room.h"
#include "player.h"
#include "item.h"
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
    // The room interior as rows of text; sprites are stamped onto it, then it is printed.
    vector<string> grid(dimensions / 2 - 1, string(dimensions - 2, ' '));

    // Stamp a sprite centered in the given cell. Spaces in a sprite are transparent.
    auto stamp = [&](const vector<string> &sprite, const vector<int> &cell)
    {
        int w = 0;
        for (const string &row : sprite)
            w = max(w, static_cast<int>(row.size()));
        int h = static_cast<int>(sprite.size());

        int x0 = cell[0] * CELL_W + (CELL_W - w) / 2;
        int y0 = cell[1] * CELL_H + (CELL_H - h) / 2;

        for (int r = 0; r < h; r++)
            for (int c = 0; c < static_cast<int>(sprite[r].size()); c++)
            {
                int gy = y0 + r, gx = x0 + c;
                if (sprite[r][c] != ' ' &&
                    gy >= 0 && gy < static_cast<int>(grid.size()) &&
                    gx >= 0 && gx < static_cast<int>(grid[gy].size()))
                    grid[gy][gx] = sprite[r][c];
            }
    };

    for (Item *item : roomItems) // items first, so the player is drawn on top
        stamp(item->getSprite(), item->getCoords());
    stamp(p->getSprite(), p->getCoords());

    for (int x = 1; x <= dimensions; x++)
        cout << "=";

    cout << endl;

    for (const string &row : grid)
        cout << "|" << row << "|\n";

    for (int x = 1; x <= dimensions; x++)
        cout << "=";

    cout << '\n';
}