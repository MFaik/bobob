#pragma once
#include <vector>

#include "map.h"
#include "program.h"

class Robot;

class Game {
private:
    Map _map;
    Program _program;
    Camera2D _camera;
    std::vector<Robot*> _robots;
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

    void tick();
    void fixed_tick();
    //TODO: find a way to remove this
    void tick_robot(Robot& robot);
    ~Game();
};
