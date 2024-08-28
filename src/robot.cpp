#include "robot.h"

#include <algorithm>
#include <iostream>

#include <map.h>

Robot::Robot(int x, int y) : _x(x), _y(y) {}

Robot::Robot(int x, int y, std::array<Item, 16> inventory) :
    _inventory(inventory), _x(x), _y(y) {}

int Robot::get_tile_parity() const {
    return _x%3+_y%3;
}

RegisterData& Robot::get_register(Register reg) {
    return this->*Robot::regs[reg];
}

void Robot::turn(Direction dir) {
    if((char)dir >= 4) {
        _dir = (Direction)(((char)_dir+((char)dir%4))%4);
    } else {
        _dir = dir;
    }
}

std::array<int, 2> Robot::get_front() const {
    switch(_dir) {
        case Direction::NORTH:
            return {_x, _y-1};
        case Direction::WEST:
            return {_x-1, _y};
        case Direction::SOUTH:
            return {_x, _y+1};
        case Direction::EAST:
            return {_x+1, _y};
        default:
            //TODO: find a better way to throw errors
            std::cout << "direction is incorrect";
    }
    return {_x, _y};
}

void Robot::go_forward(Map& map) {
    auto [x, y] = get_front();
    const auto& front_tile = map.get_tile(x, y);
    if(front_tile.robot.empty() && front_tile.get_type() != Item::BASE) {
        _x = x;
        _y = y;
    } else {
        //TODO: make this BLOCKED
        _input = 999;
    }
}

void Robot::look(Map& map) {
    auto [x, y] = get_front();
    const auto& tile = map.get_tile(x, y);
    if(tile.robot.empty())
        _input = (RegisterData)tile.get_type();
    else
        _input = (RegisterData)Item::ROBOT;
}

void Robot::use(Map& map, RegisterData sel) {
    //TODO: the use function will be changed completely
    //and use the health parameter
    auto [x, y] = get_front();
    auto& item = _inventory[std::min(sel, 16u)];
    item = map.use(x, y, item);
}

void Robot::sleep(unsigned int ticks) {
    _sleep_dur = ticks;
}

bool Robot::operator==(const Robot& other) const& {
    for(auto reg : regs) {
        if(this->*reg != other.*reg)
            return false;
    }
    return other._x == _x && other._y == _y && other._dir == _dir;
}
