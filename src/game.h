#pragma once
#include "map_elements.h"
#include <vector>

class Robot;

class Game {
private:
//TODO: remove "public:"
//add a better way to load maps
public:
    Tile *_tiles;
    Object *_objects;
    //TODO: get robots in a better memory layout
    std::vector<Robot*> _robots;
    int _x, _y;
    Game(){}
public:
    void setup(int x_size, int y_size);

    void set(int x, int y, Tile tile);
    void set(int x, int y, Object object);
    Tile get_tile(int x, int y);
    Object get_obj(int x, int y);
    void draw();

    void add_robot(int x, int y);
    Robot* get_robot(int x, int y);
    bool remove_robot(int x, int y);

    void tick();
    ~Game();
};
