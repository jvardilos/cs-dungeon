#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include "room.h"
#include "player.h"
#include "item.h"

std::vector<Room> generateMap();
Player initPlayer(std::vector<Room> &);
void turn(std::vector<Room> &);

int main()
{
    std::vector<Room> map = generateMap();
    Player p = initPlayer(map);

    std::string input;

    do
    {
        turn(map);
        std::cin >> input;

        if (input == "q")
        {
            break;
        }
    } while (true);

    return 0;
}

Room getCurrentRoom(std::vector<Room> map)
{
    for (Room r : map)
    {
        if (r.containsPlayer())
        {
            return r;
        }
    }

    std::cout << "this shouldn't happen" << std::endl;
    return NULL;
}

void turn(std::vector<Room> &map)
{
    Room r = getCurrentRoom(map);
    r.printRoom();
}

std::vector<Room> generateMap()
{
    std::vector<Room> map;

    Room r(25);

    map.push_back(r);

    return map;
}

Player initPlayer(std::vector<Room> &map)
{
    Player p;

    int dimension = map[0].getDimensions();

    map[0].updatePlayer(&p);

    p.updateCoords(dimension / 2, dimension / 4);

    return p;
}