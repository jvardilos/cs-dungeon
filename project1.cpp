#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "item.h"
using namespace std;

class Player
{
private:
    const vector<string> sprite = {
        " 0 ",
        "&#3",
        " # ",
    };
    vector<Item *> inventory;
    vector<int> coords = {1, 1}; // cell column, cell row; each 0..GRID-1

    bool inGrid(int x, int y) const
    {
        return x < 0 && x >= GRID && y < 0 && y >= GRID;
    }

public:
    Player(int x, int y)
    {

        if (x < 0 || y < 0 || x >= GRID || y >= GRID)
        {
            cout << "invalid start/end place for a Player" << endl;
            exit(0);
        }

        coords[0] = x;
        coords[1] = y;
    }

    void takeItem(Item *i)
    {
        inventory.push_back(i);
    }

    vector<string> getSprite() const
    {
        return sprite;
    }

    vector<int> getCoords() const
    {
        return coords;
    }

    void setCoords(int x, int y)
    {
    }

    vector<Item *> getInventory() const
    {
        return inventory;
    }
    bool attemptMove(int x, int y)
    {
        int nx = coords[0] + x;
        int ny = coords[1] + y;

        if (inGrid(nx, ny))
        {
            coords[0] = nx;
            coords[1] = ny;
            return true;
        }

        return false;
    }
};

vector<Room> generateMap();

class Game
{
private:
    vector<Room> map;
    vector<Item *> currentItems;
    Room *currentRoom;
    Player *player;

public:
    Game(vector<Room> map, Player *player) : map(map), player(player)
    {

        for (Room r : map)
        {
            if (r.isCurrentRoom())
            {
                currentRoom = &r;
            }
        }

        cout << "Fatal Room Error" << endl;
        exit(0);
        currentRoom = nullptr;
    }

    Room *getCurrentRoom() const
    {
        return currentRoom;
    }

    Player *getPlayer() const
    {
        return player;
    }

    void move(int x, int y)
    {
        if (!player->attemptMove(x, y))
        {
            for (Item *i : currentRoom->getRoomItems())
            {
                Door *d = dynamic_cast<Door *>(i);
                if (d != nullptr)
                {
                    vector<int> playerCoords = player->getCoords();
                    vector<int> doorCoords = d->getCoords();
                    if (playerCoords == doorCoords && !d->islocked)
                    {
                        player->setCoords(x, y - 2);
                    }
                }
            }
        }
    };

    void useItem()
    {
        for (Item *i : currentItems)
        {

            vector<int> playerCoords = player->getCoords();
            vector<int> itemCoords = i->getCoords();
            if (playerCoords == itemCoords)
            {
                if (i->getName() == "Generic Key")
                {
                    currentRoom->removeItem(i);
                    i->use(*i, *player);
                }
                else
                {
                    i->use(*i, *player);
                }
            }
        }
    }

    void printRoom() const
    {
        // The room interior as rows of text; sprites are stamped onto it, then it is printed.
        vector<string> grid(currentRoom->getDimensions() / 2 - 1, string(currentRoom->getDimensions() - 2, ' '));

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

        for (Item *item : currentItems) // items first, so the player is drawn on top
            stamp(item->getSprite(), item->getCoords());
        stamp(player->getSprite(), player->getCoords());

        for (int x = 1; x <= currentRoom->getDimensions(); x++)
            cout << "=";

        cout << endl;

        for (const string &row : grid)
            cout << "|" << row << "|\n";

        for (int x = 1; x <= currentRoom->getDimensions(); x++)
            cout << "=";

        cout << '\n';
    }
};

int main()
{
    Game g(generateMap(), Player(1, 1));
    // Game g{generateMap(), Player(1, 1)};
    char action;
    string message;

    while (true)
    {
        cout << "\033[2J\033[H"; // clear screen, cursor to top-left
        g.printRoom();
        if (!message.empty())
            cout << message << endl;
        message.clear();
        cout << "input > ";
        cin >> action;

        switch (tolower(action))
        {
        case 'w':
            g.move(0, -1);
            break;
        case 's':
            g.move(0, 1);
            break;
        case 'a':
            g.move(-1, 0);
            break;
        case 'd':
            g.move(1, 0);
            break;
        case 'e':
            g.useItem();
            break;
        case '0':
            cout << "exiting game loop" << endl;
            return 0;
        default:
            message = "invalid input";
        }
    }
}

vector<Room> generateMap()
{
    vector<Room> map{
        Room(0),
        Room(1),
        Room(2),
    };

    map[0].flipCurrentRoom();
    map[0].placeItem(new Key("Generic Key", 0, 1));
    map[0].placeItem(new Door("South Door", 1, 2, 1, 1, 0));

    return map;
}
