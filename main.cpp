#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "room.h"
#include "player.h"
#include "item.h"
using namespace std;

int main()
{
    // vector<Room> map = generateMap();

    Player p(1, 1);

    Room r;

    r.updatePlayer(&p);

    r.printRoom();
}

vector<Room> generateMap()
{
    std::vector<Room> map;

    Brazier topLeftBrazier(0, 1);
    Room r;

    r.populateItem(&topLeftBrazier);

    map.push_back(r);

    return map;
}