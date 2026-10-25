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
        " # "};
    vector<Item *> inventory;
    vector<int> coords = {0, 0};

public:
    void takeItem(Item *);
    void useItem(Item *, Room &);
    void renderSprite();
    void updateCoords(int x, int y);
    vector<int> getCoords();
};

#endif;