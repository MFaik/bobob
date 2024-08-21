#include "robot.h"
#include <algorithm>
#include <iostream>

#include "game.h"
extern Game g_game;

Robot::Robot(int x, int y) : _x(x), _y(y) {}

Robot::Robot(int x, int y, std::array<Item, 16> inventory) :
    _inventory(inventory), _x(x), _y(y) {}

int Robot::get_tile_parity() const {
    return _x%3+_y%3;
}

unsigned int& Robot::get_register(Register reg) {
    return this->*Robot::regs[reg];
}

void Robot::turn(Direction dir) {
    if(dir >= 4) {
        _dir = (Direction)((_dir+(dir%4))%4);
    } else {
        _dir = dir;
    }
}

std::array<int, 2> Robot::get_front() const {
    switch(_dir) {
        case NORTH:
            return {_x, _y-1};
        case WEST:
            return {_x-1, _y};
        case SOUTH:
            return {_x, _y+1};
        case EAST:
            return {_x+1, _y};
        default:
            //TODO: find a better way to throw errors
            std::cout << "direction is incorrect";
    }
    return {_x, _y};
}

void Robot::go_forward() {
    auto [x, y] = get_front();
    const auto& front_tile = g_game.get_tile(x, y);
    if(front_tile.robot.empty()) {
        _x = x;
        _y = y;
        _input = (int)g_game.get_tile(x, y).type;
    } else {
        //TODO: add proper input id for objects and robot
        //TODO: make this BLOCKED
        _input = 999;
    }
}

void Robot::look() {
    auto [x, y] = get_front();
    const auto& tile = g_game.get_tile(x, y);
    if(tile.robot.empty())
        _input = (short int)g_game.get_tile(x, y).type;
    else
        _input = (short int)Item::ROBOT;
}

void Robot::select(unsigned int i) {
    if(i < sizeof(_inventory)/sizeof(_inventory[0])) {
        _sel = i;
        _input = (short int)_inventory[std::min(i, 16u)];
    } else {
        _input = 0;
    }
}

void Robot::use() {
    //TODO: the use function will be changed completely
    //and use the health parameter
    auto [x, y] = get_front();
    auto& item = _inventory[std::min(_sel, 16u)];
    item = g_game.use(x, y, item);
}

void Robot::sleep(unsigned int ticks) {
    _sleep_dur = ticks;
}
