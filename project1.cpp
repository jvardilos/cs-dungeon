#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Game sizes
const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

class Player;

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
    virtual vector<string> getSprite() const = 0; // rows of text, drawn centered in the item's cell

    void updateCoords(int x, int y)
    {
        x = coords[0] + x;
        y = coords[1] + y;
        if (!(x < 0 || x >= GRID || y < 0 || y >= GRID))
        {
            coords[0] = x;
            coords[1] = y;
        }
    }

    string getName()
    {
        return name;
    }

    vector<int> getCoords()
    {
        return coords;
    }
};

struct Door : public Item
{
    int destRoom;
    int destX, destY;
    bool islocked = true;
    vector<string> opened = {
        "=   =",
        "=   =",
        "=   =",
    };

    Door(string name, int x, int y, int destRoom, int destX, int destY)
        : Item(name, x, y), destRoom(destRoom), destX(destX), destY(destY) {}

    Door(string name, int x, int y, int destRoom, int destX, int destY, bool locked)
        : Item(name, x, y), destRoom(destRoom), destX(destX), destY(destY), islocked(locked) {}

    virtual void use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        return opened;
    }
};

struct Key : public Item
{
    string sprite = "0==m";

    Key(string name, int x, int y) : Item(name, x, y) {}

    virtual void use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

class Room
{
private:
    int roomID;
    const int dimensions = 3 * CELL_W;
    vector<Item *> roomItems;
    bool current = false;

public:
    Room(int id) : roomID(id) {}

    void flipCurrentRoom()
    {
        current = !current;
    }

    bool isCurrentRoom() const
    {
        return current;
    }

    void placeItem(Item *item)
    {
        roomItems.push_back(item);
    }

    void removeItem(Item *item)
    {
        roomItems.erase(remove(roomItems.begin(), roomItems.end(), item), roomItems.end());
    }

    vector<Item *> getRoomItems() const
    {
        return roomItems;
    }

    int getRoomId() const
    {
        return roomID;
    }

    int getDimensions() const
    {
        return dimensions;
    }
};

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

    bool inGrid(int x, int y) const
    {
        return x >= 0 && x < GRID && y >= 0 && y < GRID;
    }

public:
    Player(int x, int y)
    {

        if (x < 0 || y < 0 || x >= GRID || y >= GRID)
        {
            cout << "invalid start/end place for a Player" << endl;
            exit(0);
        }

        coords[0] = x;
        coords[1] = y;
    }

    void takeItem(Item *i)
    {
        inventory.push_back(i);
    }

    vector<string> getSprite() const
    {
        return sprite;
    }

    vector<int> getCoords() const
    {
        return coords;
    }

    void setCoords(int x, int y)
    {
        coords[0] = x;
        coords[1] = y;
    }

    vector<Item *> getInventory() const
    {
        return inventory;
    }

    bool attemptMove(int x, int y)
    {
        int nx = coords[0] + x;
        int ny = coords[1] + y;

        if (inGrid(nx, ny))
        {
            coords[0] = nx;
            coords[1] = ny;
            return true;
        }

        return false;
    }
};

inline void Door::use(Item &roomItem, Player &p)
{
    for (Item *i : p.getInventory())
    {
        if (roomItem.getName() == name && i->getName() == "Generic Key")
        {
            islocked = false;
            cout << name << " has been unlocked with " << i->getName() << endl;
        }
    }
}

inline void Key::use(Item &roomItem, Player &p)
{
    if (name == "Generic Key")
    {
        p.takeItem(&roomItem);

        cout << roomItem.getName() << " was picked up and added to player's inventory" << endl;
    }
}

// ---------------------------------------------------------------- Game

class Game
{
private:
    vector<Room> map;
    Room *currentRoom = nullptr; // points into map
    Player player;

    void changeRoom(int nextRoomId)
    {
        currentRoom->flipCurrentRoom();

        for (Room &r : map)
        {
            if (r.getRoomId() == nextRoomId)
            {
                currentRoom = &r;
                break;
            }
        }
    }

public:
    Game(vector<Room> rooms, Player startPlayer) : map(rooms), player(startPlayer)
    {

        for (Room &r : map) // reference: point at the room in map, not a copy
        {
            if (r.isCurrentRoom())
            {
                currentRoom = &r;
                return;
            }
        }

        cout << "Fatal Room Error" << endl;
        exit(0);
    }

    Room *getCurrentRoom() const
    {
        return currentRoom;
    }

    Player &getPlayer()
    {
        return player;
    }

    void move(int x, int y)
    {
        if (!player.attemptMove(x, y))
        {
            for (Item *i : currentRoom->getRoomItems())
            {
                Door *d = dynamic_cast<Door *>(i);
                if (d != nullptr)
                {
                    vector<int> playerCoords = player.getCoords();
                    vector<int> doorCoords = d->getCoords();
                    if (playerCoords == doorCoords && !d->islocked)
                    {
                        changeRoom(d->destRoom);

                        cout << "dest x y " << d->destX << " " << d->destY << endl;
                        player.setCoords(d->destX, d->destY);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    void useItem()
    {
        for (Item *i : currentRoom->getRoomItems())
        {

            vector<int> playerCoords = player.getCoords();
            vector<int> itemCoords = i->getCoords();
            if (playerCoords == itemCoords)
            {
                if (i->getName() == "Generic Key")
                {
                    currentRoom->removeItem(i);
                    i->use(*i, player);
                }
                else
                {
                    i->use(*i, player);
                }
            }
        }
    }

    void printRoom() const
    {
        // The room interior as rows of text; sprites are stamped onto it, then it is printed.
        vector<string> grid(currentRoom->getDimensions() / 2 - 1, string(currentRoom->getDimensions() - 2, ' '));

        // Stamp a sprite centered in the given cell. Spaces in a sprite are transparent.
        auto stamp = [&](const vector<string> &sprite, const vector<int> &cell)
        {
            int w = 0;
            for (const string &row : sprite)
                w = max(w, static_cast<int>(row.size()));
            int h = static_cast<int>(sprite.size());

            int x0 = cell[0] * CELL_W + (CELL_W - w) / 2;
            int y0 = cell[1] * CELL_H + (CELL_H - h) / 2;

            for (int r = 0; r < h; r++)
                for (int c = 0; c < static_cast<int>(sprite[r].size()); c++)
                {
                    int gy = y0 + r, gx = x0 + c;
                    if (sprite[r][c] != ' ' &&
                        gy >= 0 && gy < static_cast<int>(grid.size()) &&
                        gx >= 0 && gx < static_cast<int>(grid[gy].size()))
                        grid[gy][gx] = sprite[r][c];
                }
        };

        for (Item *item : currentRoom->getRoomItems()) // items first, so the player is drawn on top
            stamp(item->getSprite(), item->getCoords());
        stamp(player.getSprite(), player.getCoords());

        for (int x = 1; x <= currentRoom->getDimensions(); x++)
            cout << "=";

        cout << endl;

        for (const string &row : grid)
            cout << "|" << row << "|\n";

        for (int x = 1; x <= currentRoom->getDimensions(); x++)
            cout << "=";

        cout << '\n';
    }
};

// In the beginning, render the map, this is where the game design goes.
vector<Room> generateMap()
{
    vector<Room> map{
        Room(0),
        Room(1),
        Room(2),
    };

    map[0].flipCurrentRoom();
    map[0].placeItem(new Key("Generic Key", 0, 1));
    map[0].placeItem(new Door("South Door", 1, 2, 1, 1, 0));
    map[1].placeItem(new Door("North Door", 1, 0, 0, 1, 2, false));

    return map;
}

int main()
{
    Game g(generateMap(), Player(1, 1));
    char action;
    string message;

    while (true)
    {
        // Clear screen
        cout << "\033[2J\033[H";
        g.printRoom();
        if (!message.empty())
            cout << message << endl;
        message.clear();
        cout << "input > ";
        cin >> action;

        switch (tolower(action))
        {
        case 'w':
            g.move(0, -1);
            break;
        case 's':
            g.move(0, 1);
            break;
        case 'a':
            g.move(-1, 0);
            break;
        case 'd':
            g.move(1, 0);
            break;
        case 'e':
            g.useItem();
            break;
        case '0':
            cout << "exiting game loop" << endl;
            return 0;
        default:
            message = "invalid input";
        }
    }
}
