#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "player.h"
// #include "item.h"
#include "controls.h"
using namespace std;

vector<Room> generateMap();

int main()
{
    vector<Room> map = generateMap();

    // r.updatePlayer(&p);

    // r.printRoom();

    Room r = map[0];

    controls(&r);
}

vector<Room> generateMap()
{
    vector<Room> map;

    // Brazier topLeftBrazier(0, 1);
    Room r;

    Player p(1, 1);

    r.updatePlayer(&p);

    // r.populateItem(&topLeftBrazier);

    map.push_back(r);

    return map;
}