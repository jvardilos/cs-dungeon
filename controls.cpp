#include <iostream>
#include "room.h"
#include "player.h"
using namespace std;

void controls(Room *r, Player *p)
{
    char action;
    while (true)
    {
        r->printRoom(p);
        cout << "input > ";
        cin >> action;

        switch (action)
        {
        case 'w':
            p->move(0, -1);
            break;
        case 's':
            p->move(0, 1);
            break;
        case 'a':
            p->move(-1, 0);
            break;
        case 'd':
            p->move(1, 0);
            break;
        case 'q':
            cout << "exiting game loop" << endl;
            return;
        default:
            cout << "invalid input" << endl;
        }
    }
}
