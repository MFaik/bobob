#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

#include "util.h"

class Robot;

//TODO: check if seperating the tile enum data and health into 
//seperate arrays can help memory layout
struct Tile {
public:
    enum Type {
        GRASS,
        STONE,
        PATH,
        TREE,
        COAL_MINE,
        IRON_MINE,
    } _type;
    int _health = 0;
};


constexpr int CHUNK_SIZE = 32;

struct Chunk {
    Tile _tiles[CHUNK_SIZE*CHUNK_SIZE];
    
    //TODO: just allocating robots in heap may create
    //locality and fragmentation problems
    std::vector<Robot*> _robots;
};

class Map {
    //TODO: just allocating chunks in heap may create
    //locality and fragmentation problems
    std::unordered_map<std::array<int, 2>, Chunk*> _chunks;

    void draw(Tile::Type tile_type);
private:
    void load_chunk(int x, int y);
    void unload_chunk(int x, int y);
public:
    void clean_chunks();

private:
    inline int global_to_local(int x, int y);
    Chunk& get_chunk(int x, int y);
public:
    void draw(int x, int y);
    Tile get_tile(int x, int y);
    void set_tile(int x, int y, Tile tile);


    void add_robot(int x, int y, Robot *robot);
    Robot* get_robot(int x, int y);
    void remove_robot(int x, int y);
    void move_robot(Chunk &last_chunk, Robot *robot);

    void tick();
};
