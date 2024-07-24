#include "game.h"
#include "robot.h"

Game g_game;

void Game::setup(int x_size, int y_size) {
    for(auto robot : _robots) {
        free(robot);
    }
    _robots.clear();
    if(_tiles)
        free(_tiles);
    if(_objects)
        free(_objects);
    _x = x_size;
    _y = y_size;
    _tiles = (Tile*)calloc(_x*_y, sizeof(Tile));
    _objects = (Object*)calloc(_x*_y, sizeof(Object));
}


void Game::set(int x, int y, Tile tile) {
    _tiles[x+y*_x] = tile;
}
void Game::set(int x, int y, Object object) {
    _objects[x+y*_x] = object;
}
Tile Game::get_tile(int x, int y) {
    return _tiles[x+y*_x];
}
Object Game::get_obj(int x, int y) {
    return _objects[x+y*_x];
}
void Game::draw() {
    for(int y = 0;y < _y;y++) {
        for(int x = 0;x < _x;x++) {
            Robot* r = get_robot(x, y);
            if(r) {
                std::cout << 'R';
                continue;
            }
            Object o = get_obj(x, y);
            if(o != EMPTY) {
                ::draw(o);
                continue;
            }
            ::draw(get_tile(x, y));
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Game::add_robot(int x, int y) {
    Robot *robot = new Robot(x, y);
    _robots.push_back(robot);
}

Robot* Game::get_robot(int x, int y) {
    for(size_t i = 0;i < _robots.size();i++) {
        auto& robot = _robots[i];
        if(robot->_x == x && robot->_y == y) {
            return robot;
        }
    }
    return nullptr;
}

bool Game::remove_robot(int x, int y) {
    for(size_t i = 0;i < _robots.size();i++) {
        auto& robot = _robots[i];
        if(robot->_x == x && robot->_y == y) {
            free(robot);
            _robots.erase(_robots.begin()+i);
            return true;
        }
    }

    return false;
}

Game::~Game() {
    setup(0, 0);
}
