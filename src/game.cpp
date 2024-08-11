#include "game.h"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>

#include "robot.h"
#include "program_parser.h"

Game g_game;

constexpr float TILE_SIZE = 10;

Game::Game() : _robot_allocator() {
    _camera = {};
    _camera.zoom = 2;
    setup_program(parse_program(""));
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
    
    int screenTileWidth = GetScreenWidth()/(TILE_SIZE*_camera.zoom);
    int screenTileHeight = GetScreenHeight()/(TILE_SIZE*_camera.zoom);
    for(int y_off = -1;y_off <= screenTileHeight;y_off++) {
        for(int x_off = -1;x_off <= screenTileWidth;x_off++) {
            int x = _camera.target.x/TILE_SIZE + x_off;
            int y = _camera.target.y/TILE_SIZE + y_off;
            _map.draw(x, y);
        }
    }
    
    const int robot_offset = TILE_SIZE*2;
    for(auto &r_ptr : _robots) {
        auto r = _robot_allocator.get(r_ptr);
        if(r->_x*TILE_SIZE-_camera.target.x > -robot_offset && 
           r->_x*TILE_SIZE-_camera.target.x < screenTileWidth*TILE_SIZE+robot_offset && 
           r->_y*TILE_SIZE-_camera.target.y > -robot_offset && 
           r->_y*TILE_SIZE-_camera.target.y < screenTileHeight*TILE_SIZE+robot_offset) {
            DrawCircle(r->_x*TILE_SIZE+TILE_SIZE/2, 
                       r->_y*TILE_SIZE+TILE_SIZE/2, TILE_SIZE/2, WHITE);
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

void Game::tick(bool mouse) {
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
    if(!mouse)
        return;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), _camera);
        mousePos = Vector2Scale(mousePos, 1.0/TILE_SIZE);
        add_robot(mousePos.x, mousePos.y);
    }
    if(!FloatEquals(GetMouseWheelMove(), 0)) {
        float zoom_change = 1;
        if(GetMouseWheelMove() < 0 && _camera.zoom > 0.4)
            zoom_change = 0.8;
        if(GetMouseWheelMove() > 0 && _camera.zoom < 8)
            zoom_change = 1.25;

        float w = GetScreenWidth()/_camera.zoom;
        float h = GetScreenHeight()/_camera.zoom;
        _camera.zoom *= zoom_change;
        float new_w = GetScreenWidth()/_camera.zoom;
        float new_h = GetScreenHeight()/_camera.zoom;

        _camera.target = Vector2Add(_camera.target, {
            (w-new_w) * GetMousePosition().x/GetScreenWidth(),
            (h-new_h) * GetMousePosition().y/GetScreenHeight()
        });
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
