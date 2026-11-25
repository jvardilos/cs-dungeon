#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "player.h"
#include "item.h"
using namespace std;

vector<Room> generateMap();

// Change this to a class,
class Game
{
private:
    vector<Room> map;
    Room *currentRoom;
    Player player;

public:
    Game(vector<Room> map, Player player) : map(map), player(player)
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

    Player getPlayer() const
    {
        return player;
    }

    void move(int x, int y)
    {
        if (!player.attemptMove(x, y))
        {
            for (Item *i : currentRoom->getRoomItems())
            {
                Door *d = dynamic_cast<Door *>(i);
                if (d != nullptr)
                {
                    vector<int> playerCoords = player.getCoords();
                    vector<int> doorCoords = d->getCoords();
                    if (doorCoords == playerCoords && !d->islocked)
                    {
                        player.setCoords(x, y - 2);
                    }
                }
            }
        }
    };
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
        g.getCurrentRoom()->printRoom(&g.getPlayer());
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
            g.getPlayer().useItem(g.getCurrentRoom(), g.getPlayer());
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