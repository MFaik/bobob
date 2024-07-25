#pragma once
#include <vector>

#include "map.h"
#include "code_runner.h"

class Robot;

class Game {
private:
    Map map;
    Game(){}
public:
    void setup_map();
    void setup_code(std::vector<ins_t> code, std::vector<ins_t> labels);

    void set_tile(int x, int y, Tile tile);
    Tile get_tile(int x, int y);
    void draw();

    void add_robot(int x, int y);
    Robot* get_robot(int x, int y);
    void remove_robot(int x, int y);

    void tick();
    ~Game();
};
