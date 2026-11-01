#ifndef ITEM
#define ITEM
#include <iostream>
using namespace std;

class Player;

struct Item
{
    virtual ~Item() = default;
    virtual void use(Player &, Room &) = 0;
};

struct Brazier : public Item
{
    vector<int> coords = {0, 0};
    vector<string> sprite = {"   ",
                             "___",
                             "\\_/",
                             "/ \\"};

    Brazier(int x, int y)
    {
        if (x < 0 || y < 0 || x >= 3 || y >= 3)
        {
            cout << "invalid start/end place for a Brazier" << endl;
            exit(0);
        }

        coords[0] = (x * 18) + 6;
        coords[1] = ((y * 18) + 6) / 2;

        for (int c : coords)
        {
            cout << c << endl;
        }
    }

    virtual void use(Player &p, Room &r)
    {
        vector<int> playerXY = p.getCoords();
        std::cout << "use unimplemented for Torch";
    }
    bool isLit = false;
};

struct Torch : public Item
{
    virtual void use(Player &p, Room &r)
    {
        vector<int> playerXY = p.getCoords();
        std::cout << "use unimplemented for Torch";
    }
};

#endif