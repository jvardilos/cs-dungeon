#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "player.h"
// #include "item.h"
using namespace std;

vector<Room> generateMap();
Room getCurrentRoom(vector<Room>);

int main()
{

    Player p(1, 1);
    vector<Room> map = generateMap();
    char action;

    while (true)
    {
        getCurrentRoom(map).printRoom(&p);
        cout << "input > ";
        cin >> action;

        switch (action)
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
        case 'q':
            cout << "exiting game loop" << endl;
            return 0;
        default:
            cout << "invalid input" << endl;
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

    return map;
}

Room getCurrentRoom(vector<Room> map)
{
    for (Room r : map)
    {
        cout << r.isCurrentRoom() << endl;
        if (r.isCurrentRoom())
        {
            return r;
        }
    }

    cout << "Fatal Room Error" << endl;
    exit(0);
    return Room();
}