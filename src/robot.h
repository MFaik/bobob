#pragma once
#include <array>

enum Direction : char {
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
enum Item : char {
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
private:
    friend class Program;
    std::array<Item, 16> _inventory;

    //registers
    unsigned int _inventory_selector = 0;
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
        &Robot::_input, &Robot::_inventory_selector
    };

    int get_tile_parity() const;
    unsigned int& get_register(Register reg);

    void turn(Direction dir);
private:
    std::array<int, 2> get_front() const;
public:
    Robot(int x, int y);
    Robot(int x, int y, std::array<Item, 16> inventory);

    void go_forward();
    void look();
    void select(unsigned int i);
    void use();
    void sleep(unsigned int ticks);
};
