#include "game.h"
#include "robot.h"

Game g_game;
extern CodeRunner g_code_runner;
extern Map g_map;

void Game::setup_robots() {
    for(auto robot : _robots) {
        free(robot);
    }
    _robots.clear();
}

void Game::setup_map() {
    //TODO: this will probably take in a file name 
    g_map.clean_chunks();
}

void Game::setup_code(std::vector<ins_t> code, std::vector<ins_t> labels) {
    g_code_runner.setup(code, labels);
}

void Game::set_tile(int x, int y, Tile tile) {
    g_map.set_tile(x, y, tile);
}
Tile Game::get_tile(int x, int y) {
    return g_map.get_tile(x, y);
}

//TODO: add proper way to use a camera
//TODO: add screen clear or graphics
void Game::draw() {
    for(int y = -10;y < 10;y++) {
        for(int x = -10;x < 10;x++) {
            Robot* r = get_robot(x, y);
            if(r) {
                std::cout << 'R';
                continue;
            }
            g_map.draw(x, y);
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

void Game::tick() {
    //TODO: add tick function
}

Game::~Game() {
    //TODO does this even need to exist?
}
