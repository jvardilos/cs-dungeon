#ifndef ROOM
#define ROOM
#include <iostream>
#include <vector>

class Player;
struct Item;

// A room interior (54 x 27) is split into GRID x GRID cells of CELL_W x CELL_H.
const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

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