#include "game.h"

#include <raylib.h>
#include <raymath.h>

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "robot.h"
#include "program_parser.h"

#include "game_ui.h"
extern GameUI g_game_ui;

#include "assets.h"
extern Assets g_assets;

Game::Game() : _map(*this), _robot_allocator() {
    //setup_program(parse_program(""));
}

void Game::set_camera(Camera2D camera) {
    _camera = camera;
}

//TODO: add proper error handling
void Game::load_game(const std::string& save) {
    g_game_ui.reset_robot_windows();
    //load file
    const char* save_ptr = save.c_str();
    size_t start_size = 6+2*sizeof(int);
    if(save.size() < start_size) {
        std::cout << "file too small" << std::endl;
        return;
    }
    //file type check
    if(strncmp(save_ptr, "bobob", 5)) {
        std::cout << "invalid magic numbers" << std::endl;
        return;
    }
    save_ptr += 5;
    //version check
    if(*save_ptr != 0) {
        std::cout << "file version mismatch: " << *save_ptr << std::endl;
        return;
    }
    save_ptr++;
    //get count
    size_t chunk_cnt = ((unsigned int*)save_ptr)[0];
    size_t robot_cnt = ((unsigned int*)save_ptr)[1];
    save_ptr += sizeof(unsigned int)*2;
    size_t chunk_size = (sizeof(Tile)-sizeof(Tile::robot))*CHUNK_SIZE*CHUNK_SIZE;
    size_t robot_size = sizeof(Robot);
    if(save.size() < start_size+chunk_size*chunk_cnt+robot_size*robot_cnt) {
        std::cout << "file size mismatch: " << save.size() << " < " << 
            start_size+chunk_size*chunk_cnt+robot_size*robot_cnt<< std::endl;
        return;
    }
    //read chunks
    _map.clean_chunks();
    if(chunk_cnt > 64) {
        _map.resize_chunks(chunk_cnt);
    }
    for(size_t c = 0;c < chunk_cnt;c++) {
        Chunk chunk;
        std::array<int, 2> chunk_pos = *((std::array<int, 2>*)save_ptr);
        save_ptr += sizeof(std::array<int, 2>);
        for(size_t t = 0;t < sizeof(Chunk::tiles)/sizeof(Tile);t++) {
            Item type = *((Item*)save_ptr);
            save_ptr += sizeof(Item);
            TileData data =  *((TileData*)save_ptr);
            save_ptr += sizeof(TileData);
            chunk.tiles[t] = Tile(type, data);
        }
        _map.load_chunk(chunk_pos, chunk);
    }
    //read robots
    _robots.clear();
    if(robot_cnt > 64) {
        _robot_allocator.resize(robot_cnt);
    }
    for(size_t i = 0;i < robot_cnt;i++) {
        auto r = _robot_allocator.allocate();
        _robots.push_back(r);
        auto& robot = *_robot_allocator.get(r);
        robot = *((Robot*)save_ptr);
        _map.add_robot(robot._x, robot._y, r);
        save_ptr += sizeof(Robot);
    }
    //read code
    save_ptr++;
    std::string program(save_ptr);
    setup_program(parse_program(program), false);
}

template<int N>
struct Bytes : std::array<char, N> {
  operator const char*() const {
    return std::array<char, N>::data();
  }
};

template<int N>
std::ostream& operator<<(std::ostream& out, Bytes<N> src) {
  out.write(src, N);
  return out;
}

auto to_bytes(auto&& src) {
  return std::bit_cast<Bytes<sizeof(src)>>(src);
}

std::string Game::save_game() {
    std::ostringstream save;
    save << "bobob";
    save << (char)0;

    //TODO: copying every single chunk
    auto chunks = _map.get_all_chunks();
    unsigned int chunks_size = chunks.size();
    unsigned int robots_size = _robots.size();
    save << to_bytes(chunks_size) << to_bytes(robots_size);
    for(auto& c : chunks) {
        save << to_bytes(c.first);
        for(Tile t : c.second.tiles) {
            save << to_bytes(t.get_raw_type());
            save << to_bytes(t.get_raw_data());
        }
    }
    for(auto r : _robots) {
        save << to_bytes(*_robot_allocator.get(r));
    }
    save << '\n' << _program._plain_text;
    return save.str();
}

//TODO: find a better way to do this
void Game::setup_program(Program program, bool reset_robots) {
    std::swap(_program, program);
    if(reset_robots) {
        for(auto r : _robots) {
            _robot_allocator.get(r)->_pc = 0;
        }
    }
}

std::string Game::get_program() {
    return _program._plain_text;
}

const Tile& Game::get_tile(int x, int y) {
    return _map.get_tile(x, y);
}

Tile& Game::get_tile_ref(int x, int y) {
    return _map.get_tile_ref(x, y);
}

Item Game::use(int x, int y, Item item) {
    return _map.use(x, y, item);
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

Robot* Game::get_robot(ArenaPointer<Robot> robot) {
    return _robot_allocator.get(robot);
}

void Game::remove_robot(int x, int y) {
    ArenaPointer<Robot> r = _map.get_robot(x, y);
    //if the pointer still exists in the map
    //it must also exist in the _robots vector
    _robots.erase(std::find(_robots.begin(), _robots.end(), r));
    _map.remove_robot(x, y);
    //don't forget to close the robot window
    g_game_ui.remove_robot_window(_robot_allocator.get(r));

    _robot_allocator.remove(r);
}

//TODO: remove this when hud is added
Item current_item = Item::EMPTY;
//TODO: add proper way to use a camera
void Game::draw() {
    BeginMode2D(_camera);
    ClearBackground(DARKGREEN);
    
    int screenTileWidth = GetScreenWidth()/(TILE_SIZE*_camera.zoom);
    int screenTileHeight = GetScreenHeight()/(TILE_SIZE*_camera.zoom);
    //TODO: make this for loop not hacky
    for(int y_off = -1;y_off <= screenTileHeight+1;y_off++) {
        for(int x_off = -1;x_off <= screenTileWidth+1;x_off++) {
            int x = _camera.target.x/TILE_SIZE + x_off;
            int y = _camera.target.y/TILE_SIZE + y_off;
            _map.draw(x, y);
        }
    }
    //TODO: URGENT!!!! Make a better system for rendering ui
    DrawText(std::to_string(_base_items[Item::APPLE]).c_str(), 50, 25, 30, BLACK);
    DrawTexturePro(
        g_assets.get_texture(Item::APPLE, 0),
        Rectangle{0, 0, 50, 50}, 
        Rectangle{40, 40, 40, 40}, 
        Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
    );
    DrawText(std::to_string(_base_items[Item::CHARCOAL]).c_str(), 50, 60, 30, BLACK);
    DrawTexturePro(
        g_assets.get_texture(Item::CHARCOAL, 0),
        Rectangle{0, 0, 50, 50}, 
        Rectangle{40, 75, 40, 40}, 
        Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
    );
    DrawText(std::to_string(_base_items[Item::STONE]).c_str(), 50, 95, 30, BLACK);
    DrawTexturePro(
        g_assets.get_texture(Item::STONE, 0),
        Rectangle{0, 0, 50, 50}, 
        Rectangle{44, 120, 30, 30}, 
        Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
    );
    
    EndMode2D();
}

void Game::tick(bool mouse, bool keyboard) {
    if(keyboard) {
        int speed = 20/_camera.zoom;
        if(IsKeyDown(KEY_RIGHT)) {
            _camera.target.x += speed;
        }
        if(IsKeyDown(KEY_LEFT)) {
            _camera.target.x -= speed;
        }
        if(IsKeyDown(KEY_UP)) {
            _camera.target.y -= speed;
        }
        if(IsKeyDown(KEY_DOWN)) {
            _camera.target.y += speed;
        }
        if(IsKeyPressed(KEY_ZERO)) {
            current_item = Item::EMPTY;
        }
        if(IsKeyPressed(KEY_ONE)) {
            current_item = Item::ROBOT;
        }
        if(IsKeyPressed(KEY_TWO)) {
            current_item = Item::APPLE;
        }
        if(IsKeyPressed(KEY_THREE)) {
            current_item = Item::FIRE;
        }
        if(IsKeyPressed(KEY_FOUR)) {
            current_item = Item::STONE;
        }
    }
    if(mouse) {
        //TODO: fix mouse press position
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), _camera);
            mouse_pos = Vector2Scale(mouse_pos, 1.0f/TILE_SIZE);
            mouse_pos = Vector2{std::floor(mouse_pos.x), std::floor(mouse_pos.y)};
            auto& robot = _map.get_tile(mouse_pos.x, mouse_pos.y).robot;
            if(robot.empty()) {
                _map.use(mouse_pos.x, mouse_pos.y, current_item, true);
            } else {
                g_game_ui.add_robot_window(_robot_allocator.get(robot));
            }
        }
        if(!FloatEquals(GetMouseWheelMove(), 0)) {
            float zoom_change = 1;
            if(GetMouseWheelMove() < 0 && _camera.zoom > 0.4f)
                zoom_change = 0.8f;
            if(GetMouseWheelMove() > 0 && _camera.zoom < 8)
                zoom_change = 1.25f;

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
}

void Game::fixed_tick(bool ignore_pause) {
    if(_paused && !ignore_pause)
        return;
    for(ArenaPointer<Robot> r_ptr : _robots) {
        auto r = _robot_allocator.get(r_ptr);
        int x = r->_x, y = r->_y;
        _program.tick(*r, _map);
        if(x != r->_x || y != r->_y) {
            _map.remove_robot(x, y);
            _map.add_robot(r->_x, r->_y, r_ptr);
        }
    }
    _map.tick();
}

void Game::toggle_pause() {
    _paused = !_paused;
}

bool Game::is_paused() {
    return _paused;
}

bool Game::add_item_to_base(Item item) {
    //TODO: return false on unaddable items
    _base_items[item]++;
    return true;
}

Game::~Game() {
    //TODO: does this even need to exist?
}
