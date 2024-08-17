#pragma once
#include <vector>

#include "map.h"
#include "program.h"
#include "arena_allocator.h"

class Robot;

class Game {
private:
    Map _map;
    Program _program;
    Camera2D _camera;
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
    void draw();

    void add_robot(int x, int y);
    void add_robot(int x, int y, std::array<Item, 16> inventory);
    Robot* get_robot(int x, int y);
    void remove_robot(int x, int y);

    void tick(bool mouse, bool keyboard);
    void fixed_tick(bool ignore_pause = false);

    void toggle_pause();
    bool is_paused();
    ~Game();
};
