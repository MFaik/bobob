#include "game.h"
#include "robot.h"

#include <algorithm>
#include <raylib.h>

Game g_game;

Game::Game() : _robot_allocator() {
    _camera = {};
    _camera.zoom = 2;
}

void Game::setup_map() {
    //TODO: this will probably take in a file name 
    _map.clean_chunks();
}

//TODO: find a better way to do this
void Game::setup_program(Program program) {
    std::swap(_program, program);
}

Tile& Game::get_tile(int x, int y) {
    return _map.get_tile(x, y);
}

//TODO: add proper way to use a camera
//TODO: add screen clear or graphics
void Game::draw() {
    BeginMode2D(_camera);
    
    float tile_size = 10;
    int screenTileWidth = GetScreenWidth()/(tile_size*_camera.zoom);
    int screenTileHeight = GetScreenHeight()/(tile_size*_camera.zoom);
    for(int y_off = -1;y_off <= screenTileHeight;y_off++) {
        for(int x_off = -1;x_off <= screenTileWidth;x_off++) {
            int x = _camera.target.x/tile_size + x_off;
            int y = _camera.target.y/tile_size + y_off;
            _map.draw(x, y);
        }
    }
    
    const int robot_offset = tile_size*2;
    for(auto &r_ptr : _robots) {
        auto r = _robot_allocator.get(r_ptr);
        if(r->_x*tile_size-_camera.target.x > -robot_offset && 
           r->_x*tile_size-_camera.target.x < screenTileWidth*tile_size+robot_offset && 
           r->_y*tile_size-_camera.target.y > -robot_offset && 
           r->_y*tile_size-_camera.target.y < screenTileHeight*tile_size+robot_offset) {
            DrawCircle(r->_x*tile_size+tile_size/2, 
                       r->_y*tile_size+tile_size/2, tile_size/2, WHITE);
        }
    }
    
    EndMode2D();
}

//TODO: try profiling an arena allocator
void Game::add_robot(int x, int y) {
    auto r = _robot_allocator.allocate();
    auto r_ptr = _robot_allocator.get(r);
    new (r_ptr) Robot(x, y);

    _robots.push_back(r);
    _map.add_robot(x, y, r);
}

void Game::add_robot(int x, int y, std::array<Item, 16> inventory) {
    auto r = _robot_allocator.allocate();
    auto r_ptr = _robot_allocator.get(r);
    new (r_ptr) Robot(x, y, inventory);

    _robots.push_back(r);
    _map.add_robot(x, y, r);
}

Robot* Game::get_robot(int x, int y) {
    return _robot_allocator.get(_map.get_robot(x, y));
}

void Game::remove_robot(int x, int y) {
    ArenaPointer<Robot> r = _map.get_robot(x, y);
    //if the pointer still exists in the map
    //it must also exist in the _robots vector
    _robots.erase(std::find(_robots.begin(), _robots.end(), r));
    _map.remove_robot(x, y);
    _robot_allocator.remove(r);
}

void Game::tick() {
    if(IsKeyDown(KEY_RIGHT)) {
        _camera.target.x += 2;
    }
    if(IsKeyDown(KEY_LEFT)) {
        _camera.target.x -= 2;
    }
    if(IsKeyDown(KEY_UP)) {
        _camera.target.y -= 2;
    }
    if(IsKeyDown(KEY_DOWN)) {
        _camera.target.y += 2;
    }
}

void Game::fixed_tick() {
    for(ArenaPointer<Robot> r_ptr : _robots) {
        auto r = _robot_allocator.get(r_ptr);
        int x = r->_x, y = r->_y;
        _program.tick(*r);
        if(x != r->_x || y != r->_y) {
            _map.remove_robot(x, y);
            _map.add_robot(r->_x, r->_y, r_ptr);
        }
    }
    _map.tick();
}

Game::~Game() {
    //TODO: does this even need to exist?
}
