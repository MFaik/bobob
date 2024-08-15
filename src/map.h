#pragma once
#include "raylib.h"

#include <unordered_map>
#include <array>
#include <vector>

#include "arena_allocator.h"
#include "util.h"

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
enum class WorldGeneration{
    EMPTY,
    BLOCKED,
};
struct Chunk {
    std::array<Tile, CHUNK_SIZE*CHUNK_SIZE> _tiles;
    bool edited = false;
    
    void generate(WorldGeneration a);
};

class Map {
private:
    ArenaAllocator<Chunk> _chunk_allocator;
    std::unordered_map<std::array<int, 2>, ArenaPointer<Chunk>> _chunks;
    WorldGeneration _world_generation = WorldGeneration::BLOCKED;

    Color get_tile_color(Tile::Type tile_type);
    ArenaPointer<Chunk> generate_chunk(int x, int y);
    void unload_chunk(int x, int y);
public:
    void resize_chunks(size_t size);
    void load_chunk(std::array<int, 2> pos, Chunk& chunk);
    void clean_chunks();
    std::vector<std::pair<std::array<int, 2>, Chunk>> get_all_chunks();

private:
    inline int global_to_local(int x, int y);
    Chunk& get_chunk(int x, int y);
    Tile& get_tile_internal(int x, int y);
public:
    void draw(int x, int y);
    const Tile& get_tile(int x, int y);
    Tile& get_tile_ref(int x, int y);

    void add_robot(int x, int y, ArenaPointer<Robot> r);
    void remove_robot(int x, int y);
    ArenaPointer<Robot> get_robot(int x, int y);

    void tick();
};
