#pragma once

#include <array>

#include "item.h"

class Map;

enum Direction : char {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    FORWARD,
    RIGHT,
    BACK,
    LEFT,
};

class Robot {
private:
//TODO: adding this public is hacky
public:
    friend class Program;
    std::array<Item, 16> _inventory;

    //registers
    //selected item
    unsigned int _sel = 0;
    unsigned int _cond = 0;
    unsigned int _a = 0, _b = 0, _input = 0;
    unsigned int _pc = 0;

    unsigned int _sleep_dur = 0;
public:
    int _x, _y;
    Direction _dir = NORTH;

    enum Register {
        A,
        B,
        PC,
        Cond,
        Input,
        Sel,
    };
    constexpr static unsigned int Robot::* regs[6] = {
        &Robot::_a, &Robot::_b,
        &Robot::_pc, &Robot::_cond,
        &Robot::_input, &Robot::_sel
    };

    int get_tile_parity() const;
    unsigned int& get_register(Register reg);

    void turn(Direction dir);
private:
    std::array<int, 2> get_front() const;
public:
    Robot(int x, int y);
    Robot(int x, int y, std::array<Item, 16> inventory);

    void go_forward(Map& map);
    void look(Map& map);
    void select(unsigned int i);
    void use(Map& map);
    void sleep(unsigned int ticks);
};
