#pragma once
#include <array>

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
    friend class Program;
private:
    std::array<Item, 16> _inventory;

    //registers
    int _inventory_selector = 0;
    int _cond = 0;
    int _a = 0, _b = 0, _input = 0, _pc = 0;

    unsigned int _sleep_dur = 0;
public:
    enum Register {
        A,
        B,
        PC,
        Cond,
        Input,
        Sel,
    };
    constexpr static int Robot::* regs[6] = {
        &Robot::_a, &Robot::_b,
        &Robot::_pc, &Robot::_cond,
        &Robot::_input, &Robot::_inventory_selector
    };

    int _x, _y;
    Direction _dir = NORTH;
    int get_parity() const;

    void turn(Direction dir);
private:
    inline std::array<int, 2> get_front() const;
public:
    Robot(int x, int y);
    Robot(int x, int y, std::array<Item, 16> inventory);

    void go_forward();
    void look();
    void select(unsigned int i);
    void use();
    void sleep(unsigned int ticks);
};
