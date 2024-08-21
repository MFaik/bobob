#pragma once
#include "raylib.h"

#include <unordered_map>
#include <array>
#include <vector>

#include "arena_allocator.h"
#include "item.h"
#include "util.h"

class Robot;

struct TileUpdate {
    int x;
    int y;
    //these are defined in priority order
    enum Type {
        BURN,
        APPLE,
    } type;
    TileUpdate(int x, int y, Type type):x(x), y(y), type(type) {}
};

//TODO: check if seperating the tile enum data and health into 
//seperate arrays can help memory layout
struct Tile {
public:
    Item type = Item::EMPTY;
    short int data = 0;
    ArenaPointer<Robot> robot;
    Tile(Item type, short int data) : type(type), data(data) {}
    Tile(Item type): type(type) {}
    Tile(){}
    bool operator==(const Tile& tile) const&;
    bool empty() const&;
    Item get_type() const&;
    short int get_item_cnt() const&;
};

constexpr int CHUNK_SIZE = 32;
enum class WorldGeneration{
    EMPTY,
    BLOCKED,
};

struct Chunk {
    std::array<Tile, CHUNK_SIZE*CHUNK_SIZE> tiles;
    bool edited = false;
    
    void generate(WorldGeneration a);
};

class Map {
private:
    ArenaAllocator<Chunk> _chunk_allocator;
    std::unordered_map<std::array<int, 2>, ArenaPointer<Chunk>> _chunks;
    WorldGeneration _world_generation = WorldGeneration::EMPTY;

    Color get_tile_color(Tile tile_type);
    ArenaPointer<Chunk> generate_chunk(int x, int y);
    void unload_chunk(int x, int y);

    std::vector<TileUpdate> _tile_updates;
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
    Item use(int x, int y, Item item);
private:
    void push_update_around(int x, int y, TileUpdate::Type update);
    void push_update(TileUpdate update, bool first = false);
    void tick_update(TileUpdate update);
public:
    void tick();
};
