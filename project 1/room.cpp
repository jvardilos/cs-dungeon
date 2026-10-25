#include <iostream>
#include "room.h"
#include "player.h"

// Room Functions
Room::Room(int d)
{
    dimensions = d;
}

int Room::getDimensions()
{
    return dimensions;
}

bool Room::containsPlayer()
{
    return currentPlayer;
}

void Room::updatePlayer(Player *p)
{
    currentPlayer = p;
}

void Room::printRoom()
{
    std::vector<int> coords = currentPlayer->getCoords();
    for (int x = 1; x <= dimensions; x++)
    {
        std::cout << "=";
    }

    std::cout << std::endl;

    for (int y = 2; y <= (dimensions / 2); y++)
    {
        std::cout << "|";

        for (int x = 2; x < dimensions; x++)
        {
            if (coords[0] == x && coords[1] == y)
            {
                std::cout << "*";
            }
            else
            {
                std::cout << " ";
            }
        }

        std::cout << "|" << std::endl;
    }

    // for (int i = 2; i <= (dimensions / 2); i++)
    // {
    //     std::cout << "|";

    //     for (int i = 2; i < dimensions; i++)
    //     {
    //         std::cout << " ";
    //     }

    //     std::cout << "|" << std::endl;
    // }

    for (int x = 1; x <= dimensions; x++)
    {
        std::cout << "=";
    }

    std::cout << std::endl;
}