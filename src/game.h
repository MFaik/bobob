#pragma once
#include <vector>

#include "map.h"
#include "program.h"
#include "arena_allocator.h"
#include "item.h"

class Robot;

constexpr float TILE_SIZE = 20;

class Game {
private:
    Camera2D _camera;

    Map _map;

    Program _program;
    ArenaAllocator<Robot> _robot_allocator;
    std::vector<ArenaPointer<Robot>> _robots;

    bool _paused = false;
public:
    Game();
    void load_game(std::string file_name);
    void save_game(std::string file_name);
    void setup_program(Program program, bool reset_robots = true);
    std::string get_program();

    const Tile& get_tile(int x, int y);
    Tile& get_tile_ref(int x, int y);
    Item use(int x, int y, Item item);

    void add_robot(int x, int y);
    void add_robot(int x, int y, std::array<Item, 16> inventory);
    Robot* get_robot(int x, int y);
    Robot* get_robot(ArenaPointer<Robot> robot);
    void remove_robot(int x, int y);

    void draw();

    void tick(bool mouse, bool keyboard);
    void fixed_tick(bool ignore_pause = false);

    void toggle_pause();
    bool is_paused();
    ~Game();
};
