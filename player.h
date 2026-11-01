#ifndef PLAYER
#define PLAYER
#include <vector>
using namespace std;

struct Item;
class Room;

class Player
{
private:
    const vector<string> sprite = {
        " 0 ",
        "&#3",
        " # ",
    };
    vector<Item *> inventory;
    vector<int> coords = {1, 1};

public:
    Player(int x, int y);
    void takeItem(Item *);
    void useItem(Item *, Room &);
    vector<string> getSprite();
    vector<int> getCoords();
};

#endif