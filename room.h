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
    Player *currentPlayer = NULL;
    std::vector<Item *> roomItems;

public:
    Player getCurrentPlayer();
    void updatePlayer(Player *);
    void populateItem(Item *);
    bool containsPlayer();
    void printRoom();
};

#endif