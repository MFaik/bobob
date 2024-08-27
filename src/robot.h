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

typedef unsigned int RegisterData;

class Robot {
private:
//TODO: adding this public is hacky
public:
    friend class Program;
    std::array<Item, 16> _inventory;

    //registers
    RegisterData _cond = 0;
    RegisterData _a = 0, _b = 0, _c = 0;
    RegisterData _input = 0;
    RegisterData _pc = 0;

    unsigned int _sleep_dur = 0;
public:
    int _x, _y;
    Direction _dir = NORTH;

    enum Register {
        A,
        B,
        C,
        PC,
        Cond,
        Input,
    };
    constexpr static RegisterData Robot::* regs[6] = {
        &Robot::_a, &Robot::_b, &Robot::_c, 
        &Robot::_pc, &Robot::_cond, &Robot::_input
    };

    int get_tile_parity() const;
    RegisterData& get_register(Register reg);

    void turn(Direction dir);
private:
    std::array<int, 2> get_front() const;
public:
    Robot(int x, int y);
    Robot(int x, int y, std::array<Item, 16> inventory);

    void go_forward(Map& map);
    void look(Map& map);
    void use(Map& map, RegisterData i);
    void sleep(unsigned int ticks);

    bool operator==(const Robot& other);
};
