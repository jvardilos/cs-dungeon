#include <iostream>
#include "room.h"
#include "player.h"
using namespace std;

void controls(Room *r, Player *p)
{

    char action;
    do
    {
        r->printRoom(p);
        vector<int> xy = p->getCoords();
        cout << xy[0] << " xy " << xy[1] << endl;
        cout << "input > ";
        cin >> action;

        switch (action)
        {
        case 'w':
            p->updateCoords(xy[0] - 1, xy[1]);

            cout << xy[0] - 1 << " xy " << xy[1];
        case 'a':
            p->updateCoords(xy[0] - 1, xy[1]);

            cout << xy[0] - 1 << " xy " << xy[1];
        case 's':
            p->updateCoords(xy[0] - 1, xy[1]);

            cout << xy[0] - 1 << " xy " << xy[1];
        case 'd':
            p->updateCoords(xy[0] - 1, xy[1]);

            cout << xy[0] - 1 << " xy " << xy[1];
        case 'q':
            cout << "exiting game loop" << endl;
            break;
        default:
            cout << "invalid input" << endl;
        }

    } while (true);
}