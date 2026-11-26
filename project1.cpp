#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

// Game Sizes, there are 9 squares in a grid like so
//  ___________
// |___|___|___|
// |___|___|___|
// |___|___|___|
//
// internally, everything moves withing these 9 squares.

const int GRID = 3;
const int CELL_W = 18;
const int CELL_H = 9;

// Item Names
const string ORIGIN_KEY = "Starter Key";
const string CATACOMB_KEY = "Key For the Returning";
const string NO_KEY = "";
const string BONE_ARMOR = "Bone Armor";
const string SIGMAR = "Sigmar The Destroyer";

// Door States
const bool LOCKED = true;
const bool UNLOCKED = false;

// Cardinal Directions
const vector<int> NORTH = {1, 0};
const vector<int> NORTH_EAST = {2, 0};
const vector<int> EAST = {2, 1};
const vector<int> SOUTH_EAST = {2, 2};
const vector<int> SOUTH = {1, 2};
const vector<int> SOUTH_WEST = {0, 2};
const vector<int> WEST = {0, 1};
const vector<int> NORTH_WEST = {0, 0};
const vector<int> CENTER = {1, 1};

enum class Event
{
    None,
    SecretUnlocked,
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
    vector<string> closed = {
        "=====",
        "===0=",
        "=====",
    };

    Door(string name, string keyNeeded, vector<int> coords, int destRoom, vector<int> destCoords)
        : Item(name, coords[0], coords[1]), keyNeeded(keyNeeded), destRoom(destRoom), destX(destCoords[0]), destY(destCoords[1]) {}

    Door(string name, string keyNeeded, vector<int> coords, int destRoom, vector<int> destCoords, bool locked)
        : Item(name, coords[0], coords[1]), keyNeeded(keyNeeded), destRoom(destRoom), destX(destCoords[0]), destY(destCoords[1]), islocked(locked) {}

    virtual Result use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        if (islocked)
        {
            return closed;
        }
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

struct Armor : public Item
{
    vector<string> sprite = {" W ",
                             "##3",
                             "## "};

    virtual Result use(Item &, Player &) override
    {
        Result r{"", Event::None};
        return r;
    }

    Armor(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

struct Boss : public Item
{
    vector<string> sprite = {
        "      ^       ",
        "     /*\\      ",
        "    /   \\   * ",
        "  _/_\\_/_\\_ | ",
        "   | 0 0 |  | ",
        "N  &\\|3|/&  N ",
        " \\__%@%$*__/  ",
        "     $^&      ",
    };

    virtual Result use(Item &, Player &) override
    {

        Result r{"", Event::None};
        return r;
    }

    Boss(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

// A searchable pile of skulls. A pile can hide one item.
struct SkullPile : public Item
{
    Item *hiddenItem;
    string emptyMessage;
    vector<string> sprite = {
        "(00)",
        " MM ",
    };

    SkullPile(vector<int> coords, string emptyMessage, Item *hiddenItem = nullptr)
        : Item("Skull Pile", coords[0], coords[1]), hiddenItem(hiddenItem), emptyMessage(emptyMessage) {}

    virtual Result use(Item &, Player &) override; // defined after Player

    virtual vector<string> getSprite() const override
    {
        return sprite;
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
    vector<string> floorArt; // declared after dimensions, which sizes it
    string topWall, bottomWall;
    string leftWall, rightWall; // one character per floor row

    // A one-thick wall, mostly plain with the odd broken stone
    static string brokenWall(int length, char plain)
    {
        const string rubble = "#%:";
        string wall(length, plain);
        for (char &stone : wall)
            if (rand() % 6 == 0)
                stone = rubble[rand() % rubble.size()];
        return wall;
    }

public:
    Room(int id, string name)
        : roomID(id), name(name), floorArt(dimensions / 2 - 1, string(dimensions - 2, ' '))
    {
        topWall = brokenWall(dimensions, '=');
        bottomWall = brokenWall(dimensions, '=');
        leftWall = brokenWall(floorArt.size(), '|');
        rightWall = brokenWall(floorArt.size(), '|');
    }

    string getTopWall() const { return topWall; }
    string getBottomWall() const { return bottomWall; }
    char getLeftWall(int row) const { return leftWall[row]; }
    char getRightWall(int row) const { return rightWall[row]; }

    void scatterCorners(const string &chars, int count)
    {
        int h = floorArt.size();
        int w = floorArt[0].size();
        for (int i = 0; i < count; i++)
        {
            // the smaller of two rolls bunches the mess toward the corner
            int dx = min(rand() % 14, rand() % 14);
            int dy = min(rand() % 7, rand() % 7);
            int x = (rand() % 2 == 0) ? dx : w - 1 - dx; // left or right
            int y = (rand() % 2 == 0) ? dy : h - 1 - dy; // top or bottom
            floorArt[y][x] = chars[rand() % chars.size()];
        }
    }

    void scatterPieces(const vector<string> &pieces, int count)
    {
        int h = floorArt.size();
        int w = floorArt[0].size();
        for (int i = 0; i < count; i++)
        {
            string piece = pieces[rand() % pieces.size()];
            int y = rand() % h;
            int x = rand() % (w - static_cast<int>(piece.size()) + 1); // stays inside the walls
            floorArt[y].replace(x, piece.size(), piece);
        }
    }

    // Sweep one cell bare, so nothing crowds what stands there
    void clearCell(const vector<int> &cell)
    {
        for (int y = cell[1] * CELL_H; y < (cell[1] + 1) * CELL_H; y++)
            for (int x = cell[0] * CELL_W; x < (cell[0] + 1) * CELL_W; x++)
                if (y < static_cast<int>(floorArt.size()) && x < static_cast<int>(floorArt[y].size()))
                    floorArt[y][x] = ' ';
    }

    vector<string> getFloor() const
    {
        return floorArt;
    }

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
        " @ ",
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
            p.removeItem(i);
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

inline Result SkullPile::use(Item &, Player &p)
{
    if (hiddenItem == nullptr)
    {
        return {emptyMessage, Event::None};
    }
    p.takeItem(hiddenItem);
    string found = hiddenItem->getName();
    hiddenItem = nullptr; // the pile only gives up its item once
    return {"Something glints between the skulls. You found " + found + ".", Event::None};
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
        case Event::PlayerTrapped:
            // TODO: start over?
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
        vector<string> grid = currentRoom->getFloor(); // a copy, so stamping never changes the floor

        // Stamp a sprite centered in the given cell. Spaces in a sprite are transparent.
        // With sweep on, nothing is drawn: the floor under the sprite is blanked instead,
        // plus padX columns and padY rows around it.
        auto stamp = [&](const vector<string> &sprite, const vector<int> &cell, bool sweep = false, int padX = 0, int padY = 0)
        {
            int w = 0;
            for (const string &row : sprite)
                w = max(w, static_cast<int>(row.size()));
            int h = static_cast<int>(sprite.size());

            int x0 = cell[0] * CELL_W + (CELL_W - w) / 2;
            int y0 = cell[1] * CELL_H + (CELL_H - h) / 2;

            for (int r = -padY; r < h + padY; r++)
                for (int c = -padX; c < w + padX; c++)
                {
                    int gy = y0 + r, gx = x0 + c;
                    if (gy < 0 || gy >= static_cast<int>(grid.size()) ||
                        gx < 0 || gx >= static_cast<int>(grid[gy].size()))
                        continue;

                    if (sweep)
                        grid[gy][gx] = ' ';
                    else if (r >= 0 && r < h && c >= 0 && c < static_cast<int>(sprite[r].size()) && sprite[r][c] != ' ')
                        grid[gy][gx] = sprite[r][c];
                }
        };

        // First clear some breathing room around everything that matters,
        // so doors, skulls and the player don't get lost in the debris
        for (Item *item : currentRoom->getRoomItems())
            stamp(item->getSprite(), item->getCoords(), true, 3, 1);
        stamp(player.getSprite(), player.getCoords(), true, 3, 1);

        for (Item *item : currentRoom->getRoomItems()) // items first, so the player is drawn on top
            stamp(item->getSprite(), item->getCoords());
        stamp(player.getSprite(), player.getCoords(), true); // the player covers whatever is underneath
        stamp(player.getSprite(), player.getCoords());

        cout << currentRoom->getTopWall() << '\n';

        for (int row = 0; row < static_cast<int>(grid.size()); row++)
            cout << currentRoom->getLeftWall(row) << grid[row] << currentRoom->getRightWall(row) << '\n';

        cout << currentRoom->getBottomWall() << '\n';
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

    // Floor textures are rolled once here, so they stay put between turns
    for (Room &r : map)
        r.scatterCorners(".,'`:;~", 40);
    map[3].scatterPieces({"(oo)", "(xx)", "(o.)", "--=", "c=="}, 12);
    map[3].clearCell(NORTH); // keep the Catacombs door easy to see

    map[0].setCurrentRoom(true);
    map[0].placeItem(new Key(ORIGIN_KEY, WEST));

    // Catacombs challenge: the key and the armor are hidden in two of these piles
    vector<vector<int>> pileSpots = {NORTH_WEST, NORTH_EAST, WEST, SOUTH, SOUTH_EAST};
    vector<string> emptyMessages = {
        "A jaw falls off in your hand. Nothing else here.",
        "Empty sockets stare back at you.",
        "Something skitters away under the bones.",
        "Only dust and teeth.",
        "You hear a whisper: 'not this one...'",
    };
    int keyPile = rand() % pileSpots.size();
    // step 1 or more places past the key's pile, so the two never share one
    int armorPile = (keyPile + 1 + rand() % (pileSpots.size() - 1)) % pileSpots.size();
    for (int i = 0; i < static_cast<int>(pileSpots.size()); i++)
    {
        Item *hidden = nullptr;
        if (i == keyPile)
            hidden = new Key(CATACOMB_KEY, pileSpots[i]);
        else if (i == armorPile)
            hidden = new Armor(BONE_ARMOR, pileSpots[i]);
        map[3].placeItem(new SkullPile(pileSpots[i], emptyMessages[i], hidden));
    }

    map[4].placeItem(new Boss(SIGMAR, NORTH_WEST));
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
    srand(time(0)); // a different mess and key hiding spot each run
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
