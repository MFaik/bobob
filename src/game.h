#pragma once
#include <vector>

#include "map.h"
#include "program.h"

class Robot;

class Game {
private:
    Map _map;
    Program _program;
public:
    Game(){}
    void setup_map();
    void setup_program(Program program);

    void set_tile(int x, int y, Tile tile);
    Tile get_tile(int x, int y);
    void draw();

    void add_robot(int x, int y);
    void add_robot(int x, int y, std::array<Item, 16> inventory);
    Robot* get_robot(int x, int y);
    void remove_robot(int x, int y);

    void tick();
    void tick_robot(Robot& robot);
    ~Game();
};
