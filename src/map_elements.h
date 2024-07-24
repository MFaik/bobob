#pragma once
#include <iostream>

enum Tile {
    GRASS = 1,
    STONE = 2,
    PATH = 3,
};
constexpr char tile_char[3] = {
    ',',
    '.',
    'o',
};
void draw(Tile tile);

enum Object {
    EMPTY = 0,
    TREE = 4,
    COAL_MINE = 5,
    IRON_MINE = 6,
    FURNACE = 31,
};
constexpr char obj_char[5] = {
    ' ',
    'T',
    'c',
    'i',
    'D',
};
void draw(Object object);
