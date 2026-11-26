#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Game sizes
const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

const string ORIGIN_KEY = "Starter Key";
const string CATACOMB_KEY = "Key For the Returning";
const string NO_KEY = "";

const bool LOCKED = true;
const bool UNLOCKED = false;
const vector<int> NORTH = {1, 0};
const vector<int> EAST = {2, 1};
const vector<int> SOUTH = {1, 2};
const vector<int> WEST = {0, 1};

enum class Event
{
    None,
    SecretUnlocked,
    BrazierLit,
    BossDefeated,
    PlayerTrapped
};

struct Result
{
    string message;
    Event event = Event::None;
};

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
    virtual Result use(Item &, Player &) = 0;
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
    string keyNeeded;
    int destX, destY;
    bool islocked = true;
    vector<string> opened = {
        "=   =",
        "=   =",
        "=   =",
    };

    Door(string name, string keyNeeded, vector<int> coords, int destRoom, vector<int> destCoords)
        : Item(name, coords[0], coords[1]), keyNeeded(keyNeeded), destRoom(destRoom), destX(destCoords[0]), destY(destCoords[1]) {}

    Door(string name, string keyNeeded, vector<int> coords, int destRoom, vector<int> destCoords, bool locked)
        : Item(name, coords[0], coords[1]), keyNeeded(keyNeeded), destRoom(destRoom), destX(destCoords[0]), destY(destCoords[1]), islocked(locked) {}

    virtual Result use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        return opened;
    }
};

struct Key : public Item
{
    string sprite = "0==m";

    Key(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

    virtual Result use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

class Room
{
private:
    int roomID;
    string name;
    const int dimensions = 3 * CELL_W;
    vector<Item *> roomItems;
    bool currentRoom = false;

public:
    Room(int id, string name) : roomID(id), name(name) {}

    void setCurrentRoom(bool status)
    {
        currentRoom = status;
    }

    bool isCurrentRoom() const
    {
        return currentRoom;
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

    string getName() const
    {
        return name;
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

    void removeItem(Item *item)
    {
        inventory.erase(remove(inventory.begin(), inventory.end(), item), inventory.end());
    }

    string printItems()
    {
        string message = "";
        if (inventory.size() >= 1)
        {
            for (int i = 0; i < inventory.size() - 1; i++)
            {
                message += inventory[i]->name + ", ";
            }
            message += inventory[inventory.size() - 1]->name;
        }

        return message;
    }
};

inline Result Door::use(Item &, Player &p)
{
    Result r{"", Event::None};
    for (Item *i : p.getInventory())
    {
        if (i->getName() == ORIGIN_KEY && keyNeeded == i->getName())
        {
            islocked = false;
            r.message = name + " has been unlocked with " + i->getName() + ".";
            delete i;
            return r;
        }
        if (i->getName() == CATACOMB_KEY && keyNeeded == i->getName())
        {
            islocked = false;
            r.event = Event::SecretUnlocked;
            r.message = name + " has been unlocked with " + i->getName() + ".";
            return r;
        }
    }
    // The Player has reached a game breaking state, signal event
    if (keyNeeded == CATACOMB_KEY)
    {
        r.event = Event::PlayerTrapped;
        r.message = "Despair overtakes you, you are trapped";
    }
    else
    {
        r.message = "Could not unlock " + name + ", look for a key.";
    }
    return r;
}

inline Result Key::use(Item &roomItem, Player &p)
{
    Result r{"", Event::None};
    if (name == ORIGIN_KEY)
    {
        p.takeItem(&roomItem);
        r.message = roomItem.getName() + " was picked up.";
        return r;
    }
    if (name == CATACOMB_KEY)
    {
        p.takeItem(&roomItem);
        r.message = roomItem.getName() + " was picked up.";
        return r;
    }
    return r;
}

void connectRooms(Room &origin, Room &dest, string keyNeeded, vector<int> coords, vector<int> destCoords, bool lockFromOutside, bool unlocked)
{
    origin.placeItem(new Door(origin.getName(), keyNeeded, coords, dest.getRoomId(), destCoords, lockFromOutside && unlocked));
    dest.placeItem(new Door(origin.getName(), keyNeeded, destCoords, origin.getRoomId(), coords, !lockFromOutside && unlocked));
}

class Game
{
private:
    vector<Room> map;
    Room *currentRoom = nullptr; // points into map
    Player player;

    void changeRoom(int nextRoomId)
    {
        currentRoom->setCurrentRoom(false);

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

    void openSecretRoom()
    {
        connectRooms(map[0], map[5], ORIGIN_KEY, NORTH, NORTH, false, UNLOCKED);
    }

    void handle(Result &r)
    {
        switch (r.event)
        {
        case Event::SecretUnlocked:
            openSecretRoom();
            break;
        default:
            break;
        }
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
                        return;
                    }
                }
            }
        }
    }

    string useItem()
    {
        Result r{"", Event::None};
        for (Item *i : currentRoom->getRoomItems())
        {

            vector<int> playerCoords = player.getCoords();
            vector<int> itemCoords = i->getCoords();
            if (playerCoords == itemCoords)
            {
                if (i->getName() == ORIGIN_KEY)
                {
                    currentRoom->removeItem(i);
                    r = i->use(*i, player);
                    handle(r);
                    return r.message;
                }
                else if (i->getName() == CATACOMB_KEY)
                {
                    currentRoom->removeItem(i);
                    r = i->use(*i, player);
                    handle(r);
                    return r.message;
                }
                else
                {
                    r = i->use(*i, player);
                    handle(r);
                    return r.message;
                }
            }
        }
        return r.message;
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

vector<Room> generateMap()
{

    int origin = 27;
    int crypt = 64;
    int passageway = 89;
    int catacombs = 45;
    int boss = 3;
    int hidden = 23;
    int final = 25;

    vector<Room> map{
        Room(origin, "The Origin"),
        Room(crypt, "The Crypt"),
        Room(passageway, "Passageway"),
        Room(catacombs, "Catacombs"),
        Room(boss, "Boss Battle"),
        Room(hidden, "Secret Room"),
        Room(final, "The End")};

    map[0].setCurrentRoom(true);
    map[0].placeItem(new Key(ORIGIN_KEY, WEST));
    map[3].placeItem(new Key(CATACOMB_KEY, SOUTH));
    connectRooms(map[0], map[1], ORIGIN_KEY, SOUTH, NORTH, true, LOCKED);
    connectRooms(map[1], map[2], CATACOMB_KEY, WEST, EAST, false, LOCKED);
    connectRooms(map[1], map[3], NO_KEY, SOUTH, NORTH, false, UNLOCKED);
    connectRooms(map[2], map[4], NO_KEY, WEST, EAST, false, UNLOCKED);

    return map;
}

void showTutorial()
{
    // Clear screen
    cout << "\033[2J\033[H";
    string action;
    cout << "Welcome to my text based game!" << endl;

    cout << "here are the controls" << endl;
    cout << "Use \"WASD\" to select a room, then hit enter";
    cout << "action > w // enter" << endl;
    cout << endl;
    cout << endl;
    cout << "You can chain actions to move faster throughout the game, and skip frames" << endl;
    cout << "action > ww // moves up twice" << endl;
    cout << endl;
    cout << endl;
    cout << "use \"e\" to interact with objects in the game" << endl;
    cout << "hit enter to start game!" << endl;
    getline(cin, action);
}

int main()
{
    showTutorial();

    Result r{"", Event::None};
    Game g(generateMap(), Player(1, 1));
    char action;
    string message;

    while (true)
    {
        // Clear screen
        cout << "\033[2J\033[H";
        g.printRoom();
        cout << message << endl;
        message.clear();

        Player *p = &g.getPlayer();
        Room *cr = g.getCurrentRoom();
        cout << "Room " << cr->getRoomId() << ": " << cr->getName() << endl;
        cout << "Inventory: " << p->printItems() << endl;
        cout << endl;
        cout << "action > ";
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
            message = g.useItem();
            break;
        case 'h':
            showTutorial();
            break;
        case '0': // before turn in use "q"
            cout << "exiting game loop" << endl;
            return 0;
        default:
            message = "invalid input";
        }
    }
}
