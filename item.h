#ifndef ITEM
#define ITEM
#include "player.h"
#include "room.h"
using namespace std;

struct Item
{
    vector<int> coords;

    Item(int x, int y) : coords{x, y} {};
    virtual ~Item() = default;
    // virtual void use(Player &);
    void updateCoords(int, int);
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

    Door(int, int);

    // void use(Player &) override;

    void openDoor(Player &);
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