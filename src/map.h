#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

#include "arena_allocator.h"

#include "util.h"
#include "raylib.h"

class Robot;

//TODO: check if seperating the tile enum data and health into 
//seperate arrays can help memory layout
struct Tile {
public:
    enum Type : short int {
        GRASS,
        STONE,
        PATH,
        TREE,
        COAL_MINE,
        IRON_MINE,
    } _type;
    short int _health = 0;
    ArenaPointer<Robot> _robot;
};

constexpr int CHUNK_SIZE = 32;

struct Chunk {
    Tile *_tiles;
    
    Chunk() {
        _tiles = (Tile*)calloc(CHUNK_SIZE*CHUNK_SIZE, sizeof(Tile));
    }
    Chunk& operator=(const Chunk& c) = delete;
    Chunk& operator=(Chunk&& c) = delete;
    ~Chunk() {
        free(_tiles); 
    }
};

class Map {
private:
    //TODO: just allocating chunks in heap may create
    //locality and fragmentation problems
    std::unordered_map<std::array<int, 2>, Chunk> _chunks;

    Color get_tile_color(Tile::Type tile_type);
    void load_chunk(int x, int y);
    void unload_chunk(int x, int y);
public:
    void clean_chunks();

private:
    inline int global_to_local(int x, int y);
    Chunk& get_chunk(int x, int y);
public:
    void draw(int x, int y);
    Tile& get_tile(int x, int y);

    void add_robot(int x, int y, ArenaPointer<Robot> r);
    void remove_robot(int x, int y);
    ArenaPointer<Robot> get_robot(int x, int y);

    void tick();
};
