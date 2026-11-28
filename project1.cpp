#include <iostream>
#include <string>
#include <vector>
#include <random>
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

// Room names
const int ORIGIN = 0;
const int CRYPT = 1;
const int PASSAGEWAY = 2;
const int CATACOMBS = 3;
const int BOSS = 4;
const int HIDDEN = 5;
const int END = 6;

// Item Names
const string ORIGIN_KEY = "Starter Key";
const string CATACOMB_KEY = "Key For the Returning";
const string NO_KEY = "";
const string SIGMAR = "Sigmar The Destroyer";
const string BANE_OF_SIGMAR = "Bane of Sigmar";
const string ALTAR = "The Escape Altar";

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

random_device myEngine;
uniform_int_distribution<int> randInt(1, 5);
int randomTaunt()
{
    return randInt(myEngine);
}

bool playing = true;

// LLM additions, makes it so one character is received per "frame", then clears screen.
// Felt it was appropriate because this is a terminal emulator game that needs
// specific functionality that is easily added by LLM and doesn't contribute to my learning.
void clearScreen()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\033[2J\033[H";
}

enum class Event
{
    None,
    SecretUnlocked,
    BossTaunt,
    BossDefeated,
    PlayerTrapped,
    BeatGame,
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
    string keyNeeded;
    int destRoom;
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

    virtual Result use(Item &, Player &p) override;

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

    virtual Result use(Item &roomItem, Player &p) override;

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

struct Boss : public Item
{
    bool isAlive = true;
    vector<string> sigmar = {
        "      ^       ",
        "     /*\\      ",
        "    /   \\   * ",
        "  _/_\\_/_\\_ | ",
        "   | 0 0 |  | ",
        "N  &\\|3|/&  N ",
        " \\__%@%$*__/  ",
        "     $^&      ",
    };

    vector<string> vanquished = {
        "* ",
        " *",
        "* "};

    virtual Result use(Item &, Player &p) override;

    Boss(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

    virtual vector<string> getSprite() const override
    {
        if (isAlive)
        {
            return sigmar;
        }
        return vanquished;
    }
};

struct Sword : public Item
{
    vector<string> sprite = {
        " | ",
        "_|_",
        " | ",
    };

    virtual Result use(Item &roomItem, Player &p) override;

    Sword(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

    virtual vector<string> getSprite() const override
    {
        return {sprite};
    }
};

struct Altar : public Item
{
    vector<string> sprite = {
        "mmmmm",
        "==0==",
        "=====",
    };

    virtual Result use(Item &roomItem, Player &p) override;

    Altar(string name, vector<int> coords) : Item(name, coords[0], coords[1]) {}

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

    const vector<string> armed = {
        "   0  /",
        "  &#3/ ",
        "   #   ",
    };
    bool isArmed = false;

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
        if (i->getName() == BANE_OF_SIGMAR)
        {
            isArmed = true;
        }
        inventory.push_back(i);
    }

    vector<string> getSprite() const
    {
        if (isArmed)
        {
            return armed;
        }
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
            for (int i = 0; i < int(inventory.size()) - 1; i++)
            {
                message += inventory[i]->name + ", ";
            }
            message += inventory[inventory.size() - 1]->name;
        }

        return message;
    }
};

void connectRooms(Room &origin, Room &dest, string keyNeeded, vector<int> coords, vector<int> destCoords, bool lockFromOutside, bool unlocked)
{
    origin.placeItem(new Door(origin.getName(), keyNeeded, coords, dest.getRoomId(), destCoords, lockFromOutside && unlocked));
    dest.placeItem(new Door(origin.getName(), keyNeeded, destCoords, origin.getRoomId(), coords, !lockFromOutside && unlocked));
}

Result Door::use(Item &, Player &p)
{
    Result r{"", Event::None};
    if (islocked)
    {
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
        }
        else
        {
            r.message = "Could not unlock " + name + ", look for a key.";
        }
    }

    return r;
};

Result Key::use(Item &roomItem, Player &p)
{
    Result r{"", Event::None};
    p.takeItem(&roomItem);
    r.message = roomItem.getName() + " was picked up.";
    return r;
};

Result Boss::use(Item &, Player &p)
{
    Result r{"", Event::None};
    if (isAlive)
    {
        for (Item *i : p.getInventory())
        {
            if (i->getName() == BANE_OF_SIGMAR)
            {
                r.event = Event::BossDefeated;
                isAlive = false;
                r.message = "Sigmar has been defeated... For now.";
                return r;
            }
        }
        r.event = Event::BossTaunt;
    }
    else
    {
        r.message = "The latent magic here is strong, this is not the end of Sigmar.";
    }
    return r;
}

Result Sword::use(Item &roomItem, Player &p)
{
    Result r{"", Event::None};
    p.takeItem(&roomItem);
    r.message = roomItem.getName() + " was picked up.";
    return r;
}

Result Altar::use(Item &, Player &)
{
    Result r{"You have Completed the Game!", Event::BeatGame};
    return r;
}

class Game
{
private:
    Event currentEvent;
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

    Event getCurrentEvent() const
    {
        return currentEvent;
    }

    void openSecretRoom()
    {
        connectRooms(map[ORIGIN], map[HIDDEN], ORIGIN_KEY, NORTH, NORTH, false, UNLOCKED);
    }

    void taunt(Result &r)
    {
        currentEvent = Event::BossTaunt;
        int tauntId = randomTaunt();

        switch (tauntId)
        {
        case 1:
            r.message = "Your Mother was a hamster, and your father smelt of elderberries!";
            break;
        case 2:
            r.message = "I fart in your general direction!";
            break;
        case 3:
            r.message = "Go and boil your bottom, son of a silly person!";
            break;
        case 4:
            r.message = "You empty-headed animal food trough wiper!";
            break;
        case 5:
            r.message = "I blow my nose at you, so-called Arthur-king, you and all your silly English... Kuh-nigets";
            break;
        default:
            r.message = "idk what happened";
            break;
        }
    }

    void openEnd()
    {
        currentEvent = Event::BossDefeated;
        connectRooms(map[BOSS], map[END], ORIGIN_KEY, WEST, EAST, false, UNLOCKED);
    }

    void trapped(Result &r)
    {
        currentEvent = Event::PlayerTrapped;
        r.message = "Despair overtakes you, you are trapped";
    }

    void exitGame()
    {
        currentEvent = Event::BeatGame;
    }

    void handle(Result &r)
    {
        switch (r.event)
        {
        case Event::SecretUnlocked:
            openSecretRoom();
            break;
        case Event::BossTaunt:
            taunt(r);
            break;
        case Event::BossDefeated:
            openEnd();
            break;
        case Event::PlayerTrapped:
            trapped(r);
            break;
        case Event::BeatGame:
            exitGame();
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
                else if (i->getName() == BANE_OF_SIGMAR)
                {
                    currentRoom->removeItem(i);
                    r = i->use(*i, player);
                    handle(r);
                    return r.message;
                }
                else if (i->getName() == SIGMAR)
                {
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

    // LLM Addition, renders my sprites in a 3x3 space.
    // This is that core feature that is asked for in the assignment, a full graphical "map".
    // Originally I created it, but as I needed to put sprites on it it got too complicated,
    // so I chose it to be my LLM candidate.
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
    vector<Room> map{
        Room(ORIGIN, "The Origin"),
        Room(CRYPT, "The Crypt"),
        Room(PASSAGEWAY, "Passageway"),
        Room(CATACOMBS, "Catacombs"),
        Room(BOSS, "Boss Battle"),
        Room(HIDDEN, "Secret Room"),
        Room(END, "The End")};

    map[ORIGIN].setCurrentRoom(true);
    map[ORIGIN].placeItem(new Key(ORIGIN_KEY, WEST));
    map[CATACOMBS].placeItem(new Key(CATACOMB_KEY, SOUTH));
    map[BOSS].placeItem(new Boss(SIGMAR, NORTH_WEST));
    map[HIDDEN].placeItem(new Sword(BANE_OF_SIGMAR, SOUTH_EAST));
    map[END].placeItem(new Altar(ALTAR, WEST));
    connectRooms(map[ORIGIN], map[CRYPT], ORIGIN_KEY, SOUTH, NORTH, true, LOCKED);
    connectRooms(map[CRYPT], map[PASSAGEWAY], CATACOMB_KEY, WEST, EAST, false, LOCKED);
    connectRooms(map[CRYPT], map[CATACOMBS], NO_KEY, SOUTH, NORTH, false, UNLOCKED);
    connectRooms(map[PASSAGEWAY], map[BOSS], NO_KEY, WEST, EAST, false, UNLOCKED);

    return map;
}

void showTutorial()
{
    clearScreen();
    string action;
    cout << "Welcome to my text based game!" << endl;

    cout << "here are the controls" << endl;
    cout << "Use \"WASD\" to select a room, then hit enter";
    cout << "action > w // enter" << endl;
    cout << endl;
    cout << endl;
    cout << "E to interact, use this on doors (or anything else you'd like to use)" << endl;
    cout << "action > e" << endl;
    cout << endl;
    cout << endl;
    cout << "H to show this help menu" << endl;
    cout << "action > h" << endl;
    cout << "Q to quit the game and return to homescreen" << endl;
    cout << "action > q" << endl;
    cout << "hit enter to start game!" << endl;
    getline(cin, action);
}

void gameLoop()
{
    bool isQuit = false;
    Result r{"", Event::None};
    Game g(generateMap(), Player(1, 1));
    char action;
    string message;

    while (g.getCurrentEvent() != Event::BeatGame || isQuit)
    {
        if (g.getCurrentEvent() == Event::PlayerTrapped)
        {
            gameLoop();
        }
        else
        {
            clearScreen();
            g.printRoom();
            cout << message << endl;
            message.clear();

            Player *p = &g.getPlayer();
            Room *cr = g.getCurrentRoom();
            cout << endl;
            cout << cr->getName() << endl;
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
            case 'q':
                cout << "exiting game loop" << endl;
                isQuit = true;
                break;
            default:
                message = "invalid input";
            }
        }
    }

    clearScreen();
    cout << endl;
    cout << endl;
    cout << "Thank you for playing my Game!" << endl;
    cout << endl;
    cout << endl;
}

int main()
{
    showTutorial();
    gameLoop();
    return 0;
}
