#pragma once
#include <array>

#include "game.h"

enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    UP,
    RIGHT,
    DOWN,
    LEFT,
};

//TODO: create an items file
enum Item {
    NONE,
    WOOD,
    COAL,
    IRON_ORE,
    IRON_BAR,
    AXE,
    PICKAXE,
    PATH_ITEM,
};

class Robot {
    friend class CodeRunner;
private:
    Item _inventory[16];

    //registers
    int _inventory_selector = 0;
    int _cond = 0;
    int _a = 0, _b = 0, _input = 0, _pointer = 0;
    int _sleep_dur = 0;

public:
    int _x, _y, _dir = NORTH;

    int get_parity() const;

    void turn(Direction dir);
private:
    inline std::array<int, 2> get_front() const;
public:
    Robot(int x, int y);

    void go_forward();
    void look();
    void select(uint i);
    void use();
    void sleep(uint i);
    void tick();
};
