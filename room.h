#ifndef ROOM
#define ROOM
#include <iostream>
#include <vector>
using namespace std;

class Player;
struct Item;

// A room interior (54 x 27) is split into GRID x GRID cells of CELL_W x CELL_H.
const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

class Room
{
private:
    const int dimensions = 3 * CELL_W;
    vector<Item *> roomItems;
    bool current = false;

public:
    void flipCurrentRoom();
    bool isCurrentRoom();
    void placeItem(Item *);
    void removeItem(Item *);
    vector<Item *> getRoomItems();
    void printRoom(Player *);
};

#endif