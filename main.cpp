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
    Player p(1, 1);
    vector<Room> map = generateMap();

    Room r = map[0];

    controls(&r, &p);
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