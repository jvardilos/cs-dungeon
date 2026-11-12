#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "player.h"
#include "item.h"
using namespace std;

vector<Room> generateMap();
Room getCurrentRoom(vector<Room>);

int main()
{
    Player p(1, 1);
    vector<Room> map = generateMap();
    char action;
    string message;

    while (true)
    {
        cout << "\033[2J\033[H"; // clear screen, cursor to top-left
        Room currentRoom = getCurrentRoom(map);
        currentRoom.printRoom(&p);
        if (!message.empty())
            cout << message << endl;
        message.clear();
        cout << "input > ";
        cin >> action;

        switch (tolower(action))
        {
        case 'w':
            p.move(0, -1);
            break;
        case 's':
            p.move(0, 1);
            break;
        case 'a':
            p.move(-1, 0);
            break;
        case 'd':
            p.move(1, 0);
            break;
        case 'e':
            p.useItem(currentRoom);
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
        Room(),
        Room(),
        Room(),
    };

    map[0].flipCurrentRoom();
    map[0].placeItem(new Door(2, 1));

    return map;
}

Room getCurrentRoom(vector<Room> map)
{
    for (Room r : map)
    {
        if (r.isCurrentRoom())
        {
            return r;
        }
    }

    cout << "Fatal Room Error" << endl;
    exit(0);
    return Room();
}