#include "robot.h"
#include "code_runner.h"

extern Game g_game;
extern CodeRunner g_code_runner;

Robot::Robot(int x, int y) {
    _x = x;
    _y = y;
}

int Robot::get_parity() const {
    return _x%3+_y%3;
}

void Robot::turn(Direction dir) {
    if(dir >= 4) {
        _dir = (_dir+(dir%4))%4;
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
    }
    return {_x, _y};
}

void Robot::go_forward() {
    auto [x, y] = get_front();
    Object obj = g_game.get_obj(x, y);
    if(obj == EMPTY) {
        _x = x;
        _y = y;
        _input = g_game.get_tile(x, y);
    } else {
        _input = obj;
    }
}

void Robot::look() {
    auto [x, y] = get_front();
    Object obj = g_game.get_obj(x, y);
    if(obj == EMPTY) {
        _input = g_game.get_tile(x, y);
    } else {
        _input = obj;
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
    switch(_inventory[_inventory_selector]) {
        case PATH_ITEM: {
            g_game.set(_x, _y, PATH);
            break;
                   }
        case AXE: {
            auto [x, y] = get_front();
            if(g_game.get_obj(x, y) == TREE) {
                g_game.set(x, y, EMPTY);
            }
            break;
                  }
        case PICKAXE: {
            auto [x, y] = get_front();
            Object obj = g_game.get_obj(x, y);
            if(obj == COAL_MINE) {
                g_game.set(x, y, EMPTY);
            } else if(obj == IRON_MINE) {
                g_game.set(x, y, EMPTY);
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

void Robot::tick() {
    if(_sleep_dur)
        _sleep_dur--;
    else
        g_code_runner.tick(*this);
}
