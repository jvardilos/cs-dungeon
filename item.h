#ifndef ITEM
#define ITEM
#include "player.h"
#include "room.h"
using namespace std;

struct Item
{
    string name;
    vector<int> coords;

    Item(string n, int x, int y)
    {
        name = n;
        coords = {x, y};
    }
    virtual ~Item() = default;
    virtual void use(Item &, Player &) = 0;
    virtual vector<string> getSprite() = 0; // rows of text, drawn centered in the item's cell
    void updateCoords(int, int);
    string getName();
    vector<int> getCoords();
};

struct Door : public Item
{
    bool islocked = true;
    vector<string> opened = {
        "=   =",
        "=   =",
        "=   =",
    };

    Door(string, int, int);

    virtual void use(Item &, Player &) override;
    virtual vector<string> getSprite() override;
};

struct Key : public Item
{
    string sprite = "0==m";
    Key(string, int, int);

    virtual void use(Item &, Player &) override;
    virtual vector<string> getSprite() override;
};

// struct Brazier : public Item
// {
//     vector<int> coords = {0, 0};
//     vector<string> sprite = {"   ",
//                              "___",
//                              "\\_/",
//                              "/ \\"};

//     Brazier(int x, int y)
//     {
//         if (x < 0 || y < 0 || x >= 3 || y >= 3)
//         {
//             cout << "invalid start/end place for a Brazier" << endl;
//             exit(0);
//         }

//         coords[0] = (x * 18) + 6;
//         coords[1] = ((y * 18) + 6) / 2;

//         for (int c : coords)
//         {
//             cout << c << endl;
//         }
//     }

//     virtual void use(Player &p, Room &r)
//     {
//         vector<int> playerXY = p.getCoords();
//         std::cout << "use unimplemented for Torch";
//     }
//     bool isLit = false;
// };

// struct Torch : public Item
// {
//     virtual void use()
//     {
//     }
// };

#endif