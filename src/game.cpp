#include "game.h"
#include "robot.h"

Game g_game;

void Game::setup_map() {
    //TODO: this will probably take in a file name 
    _map.clean_chunks();
}

void Game::setup_code(std::vector<ins_t> code, std::vector<ins_t> labels) {
    _program.setup(code, labels);
}

void Game::set_tile(int x, int y, Tile tile) {
    _map.set_tile(x, y, tile);
}
Tile Game::get_tile(int x, int y) {
    return _map.get_tile(x, y);
}

//TODO: add proper way to use a camera
//TODO: add screen clear or graphics
void Game::draw() {
    for(int y = 0;y < 100;y++) {
        for(int x = 0;x < 100;x++) {
            Robot* r = get_robot(x, y);
            if(r) {
                continue;
            }
            _map.draw(x, y);
        }
    }
}

void Game::add_robot(int x, int y) {
    Robot *robot = new Robot(x, y);
    _map.add_robot(robot);
}

void Game::add_robot(int x, int y, std::array<Item, 16> inventory) {
    Robot *robot = new Robot(x, y, inventory);
    _map.add_robot(robot);
}

Robot* Game::get_robot(int x, int y) {
    return _map.get_robot(x, y);
}

void Game::remove_robot(int x, int y) {
    _map.remove_robot(x, y);
}

void Game::tick() {
    _map.tick();
}

void Game::tick_robot(Robot& robot) {
    _program.tick(robot);
}

Game::~Game() {
    //TODO: does this even need to exist?
}
