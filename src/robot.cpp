#include "robot.h"
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
    if(!g_game.get_tile(x, y)._robot) {
        _x = x;
        _y = y;
        _input = (int)g_game.get_tile(x, y)._type;
    } else {
        //TODO: add proper input id for objects and robot
        _input = 999;
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
    auto [x, y] = get_front();
    auto &tile = g_game.get_tile(x, y);
    switch(_inventory[_inventory_selector]) {
        case PATH_ITEM:
            tile._type = Tile::PATH;
            break;
        case AXE:
            if(tile._type == Tile::TREE) {
                tile._type = Tile::GRASS;
            }
            break;
        case PICKAXE:
            if(tile._type == Tile::COAL_MINE) {
                tile._type = Tile::STONE;
            } else if(tile._type == Tile::IRON_MINE) {
                tile._type = Tile::STONE;
            }
            break;
        default:
            break;
    }
}

void Robot::sleep(unsigned int ticks) {
    _sleep_dur = ticks;
}
