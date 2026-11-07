#ifndef ROOM
#define ROOM
#include <iostream>
#include <vector>

class Player;
struct Item;

class Room
{
private:
    int dimensions = 56;
    std::vector<Item *> roomItems;
    bool current = false;

public:
    void flipCurrentRoom();
    bool isCurrentRoom();
    void populateItem(Item *);
    void printRoom(Player *);
};

#endif