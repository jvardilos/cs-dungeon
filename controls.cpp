#include <iostream>
#include "room.h"
#include "player.h"
using namespace std;

void controls(Room *r)
{

    r->printRoom();
    Player p = r->getCurrentPlayer();
    vector<int> xy = p.getCoords();

    char action;
    do
    {
        cout << "input > ";
        cin >> action;

        switch (action)
        {
        case 'w':
            p.updateCoords(xy[0], xy[1] + 1);
        case 'q':
            cout << "exiting game loop";
        }
        if (action == 'q')
        {
            cout << "exiting game loop";
            break;
        }
    } while (true);
}