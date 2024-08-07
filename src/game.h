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
public:
    Game();
    void setup_map();
    void setup_program(Program program);

    Tile& get_tile(int x, int y);
    void draw();

    void add_robot(int x, int y);
    void add_robot(int x, int y, std::array<Item, 16> inventory);
    Robot* get_robot(int x, int y);
    void remove_robot(int x, int y);

    void tick(bool mouse);
    void fixed_tick();
    ~Game();
};
