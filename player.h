#ifndef PLAYER
#define PLAYER
#include <vector>
using namespace std;

struct Item;
class Room;

// A room interior (54 x 27) is split into GRID x GRID cells of CELL_W x CELL_H.
const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

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
    void useItem(Item *, Room &);
    vector<string> getSprite();
    vector<int> getCoords();
    bool move(int, int); // dx, dy in {-1,0,1}; false if it would leave the grid
};

#endif