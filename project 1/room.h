#ifndef ROOM
#define ROOM
#include <iostream>
#include <vector>

class Player;
struct Item;

class Room
{
private:
    int dimensions = 0;
    Player *currentPlayer = NULL;
    std::vector<Item *> roomItems;

public:
    Room(int d);
    void updatePlayer(Player *);
    int getDimensions();
    bool containsPlayer();
    void printRoom();
};

#endif