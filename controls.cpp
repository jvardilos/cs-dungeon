#include <iostream>
#include "player.h"
#include "item.h"
using namespace std;

void updatePlayerPosition(Player &p)
{
}

void useItem(Player &p)
{
    p.useItem();
}

void controls()
{
    char action;
    cin >> action;

    cout << action << endl;
}