# cs-dungeon

Text adventure for CS135 Project 1. Console game written in C++.

## Build

From this directory:

```sh
g++ -std=c++17 -Wall -Wextra main.cpp room.cpp player.cpp -o dungeon
```

Add `-fsanitize=address,undefined` to catch memory bugs while testing:

```sh
g++ -std=c++17 -Wall -Wextra -fsanitize=address,undefined main.cpp room.cpp player.cpp -o dungeon
```

## Run

```sh
./dungeon
```

Use a real terminal (Terminal, iTerm, etc.). The game clears the screen with ANSI
escape codes, which some IDE output panes show as literal `[2J` text.

## Controls

Type a key and press Enter.

| Key | Action                    |
| --- | ------------------------- |
| `w` | move up                   |
| `a` | move left                 |
| `s` | move down                 |
| `d` | move right                |
| `0` | quit                      |

`e` (use) is not enabled yet.

## Files

- `main.cpp`: game loop and map setup
- `room.h` / `room.cpp`: room drawing
- `player.h` / `player.cpp`: player cell position and movement
- `item.h`: items (not implemented yet)
