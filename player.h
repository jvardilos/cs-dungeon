#ifndef PLAYER
#define PLAYER
#include <vector>
#include "item.h"
#include "room.h"
using namespace std;

class Player
{
private:
    const vector<string> sprite = {
        " 0 ",
        "&#3",
        " # ",
    };
    vector<Item *> inventory;
    vector<int> coords = {1, 1}; // cell column, cell row; each 0..GRID-1

public:
    Player(int x, int y);
    void takeItem(Item *);
    void useItem(Room &, Player &);
    vector<string> getSprite();
    vector<int> getCoords();
    vector<Item *> getInventory();
    void move(int, int);
};

#endif