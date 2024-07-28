#include "robot.h"
#include "game.h"

extern Game g_game;

Robot::Robot(int x, int y) : _x(x), _y(y) {}

Robot::Robot(int x, int y, std::array<Item, 16> inventory) :
    _inventory(inventory), _x(x), _y(y) {}

int Robot::get_parity() const {
    return _x%3+_y%3;
}

int& Robot::get_register(Register reg) {
    return this->*Robot::regs[reg];
}

void Robot::turn(Direction dir) {
    if(dir >= 4) {
        _dir = (Direction)((_dir+(dir%4))%4);
    } else {
        _dir = dir;
    }
}

inline std::array<int, 2> Robot::get_front() const {
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
    //TODO: add collision
    if(true) {
        _x = x;
        _y = y;
        _input = (int)g_game.get_tile(x, y)._type;
    } else {
        _input = 0;
    }
}

void Robot::look() {
    auto [x, y] = get_front();
    if(true) {
        _input = (int)g_game.get_tile(x, y)._type;
    } else {
        _input = 0;
    }
}

void Robot::select(unsigned int i) {
    if(i < sizeof(_inventory)/sizeof(_inventory[0])) {
        _inventory_selector = i;
        _input = _inventory[i];
    } else {
        _input = 0;
    }
}

void Robot::use() {
    //TODO: the use function will be changed completely
    //and use the health parameter
    switch(_inventory[_inventory_selector]) {
        case PATH_ITEM: {
            g_game.set_tile(_x, _y, {Tile::PATH, 0});
            break;
                   }
        case AXE: {
            auto [x, y] = get_front();
            if(g_game.get_tile(x, y)._type == Tile::TREE) {
                g_game.set_tile(x, y, {Tile::GRASS, 0});
            }
            break;
                  }
        case PICKAXE: {
            auto [x, y] = get_front();
            auto tile = g_game.get_tile(x, y);
            if(tile._type == Tile::COAL_MINE) {
                g_game.set_tile(x, y, {Tile::STONE, 0});
            } else if(tile._type == Tile::IRON_MINE) {
                g_game.set_tile(x, y, {Tile::STONE, 0});
            }
            break;
                      }
        default:
            break;
    }
}

void Robot::sleep(unsigned int ticks) {
    _sleep_dur = ticks;
}
