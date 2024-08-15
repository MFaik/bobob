#include "map.h"
#include "robot.h"
#include "game.h"

extern Game g_game;

template class ArenaAllocator<Chunk>;

Color Map::get_tile_color(Tile::Type tile_type) {
    switch (tile_type) {
        case Tile::GRASS:
            return DARKGREEN;
        case Tile::STONE:
            return DARKGRAY;
        case Tile::PATH:
            return DARKBROWN;
        case Tile::TREE:
            return BROWN;
        case Tile::COAL_MINE:
            return BLACK;
        case Tile::IRON_MINE:
            return LIGHTGRAY;
        default:
            return PINK;
    }
}

void Map::draw(int x, int y) {
    DrawRectangle(x*10, y*10, 10, 10, get_tile_color(get_tile(x, y)._type));
}

void Map::resize_chunks(size_t size) {
    _chunk_allocator.resize(size);
}

void Map::load_chunk(std::array<int, 2> pos, Chunk& chunk) {
    auto i = _chunk_allocator.allocate();
    chunk.edited = true;
    *_chunk_allocator.get(i) = std::move(chunk);
    _chunks[pos] = i;
}

//The chunk must NOT EXIST when this function is called
ArenaPointer<Chunk> Map::generate_chunk(int chunk_x, int chunk_y) {
    auto chunk = _chunk_allocator.allocate();
    _chunks[{chunk_x, chunk_y}] = chunk;
    _chunk_allocator.get(chunk)->generate(_world_generation);
    return chunk;
}

//The chunk must EXIST when this function is called
void Map::unload_chunk(int chunk_x, int chunk_y) {
    auto chunk = _chunks.find({chunk_x, chunk_y});
    _chunks.erase(chunk);
}

void Map::clean_chunks() {
    _chunk_allocator.clear();
    _chunks.clear();
}

std::vector<std::pair<std::array<int, 2>, Chunk>> Map::get_all_chunks() {
    std::vector<std::pair<std::array<int, 2>, Chunk>> ret;   
    for(auto chunk_pair : _chunks) {
        Chunk &chunk = *_chunk_allocator.get(chunk_pair.second);
        if(chunk.edited == false)
            continue;
        std::pair<std::array<int, 2>, Chunk> c_ret;
        c_ret = make_pair(chunk_pair.first, chunk);
        ret.push_back(c_ret);
    }
    return ret;
}

inline int Map::global_to_local(int x, int y) {
    return ((x%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE)*CHUNK_SIZE +
        ((y%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE);
}

//TODO: we can remove the conditional if the chunk is guaranteed to exist
//TODO: load and unload chunks depending on:
//their robot count
//neighbour robot count
//currently this is a memory leak
Chunk& Map::get_chunk(int x, int y) {
    if(x%CHUNK_SIZE && x < 0)
        x-=CHUNK_SIZE-1;
    if(y%CHUNK_SIZE && y < 0)
        y-=CHUNK_SIZE-1;
    auto chunk = _chunks.find({x/CHUNK_SIZE, y/CHUNK_SIZE});
    if(chunk == _chunks.end()) {
        auto chunk = generate_chunk(x/CHUNK_SIZE, y/CHUNK_SIZE);
        return *_chunk_allocator.get(chunk);
    }
    return *_chunk_allocator.get(chunk->second);
}

Tile& Map::get_tile_internal(int x, int y) {
    return get_chunk(x, y)._tiles[global_to_local(x, y)];
}

const Tile& Map::get_tile(int x, int y) {
    return get_chunk(x, y)._tiles[global_to_local(x, y)];
}

Tile& Map::get_tile_ref(int x, int y) {
    Chunk& chunk = get_chunk(x, y);
    chunk.edited = true;
    return chunk._tiles[global_to_local(x, y)];
}

void Map::add_robot(int x, int y, ArenaPointer<Robot> robot) {
    get_tile_internal(x, y)._robot = robot;
}

void Map::remove_robot(int x, int y) {
    get_tile_internal(x, y)._robot = ArenaPointer<Robot>();
}

ArenaPointer<Robot> Map::get_robot(int x, int y) {
    return get_tile(x, y)._robot;
}

void Map::tick() {

}

void Chunk::generate(WorldGeneration world_gen) {
    edited = false;
    switch(world_gen) {
        case WorldGeneration::EMPTY:
            for(Tile &t : _tiles) {
                t._type = Tile::GRASS;
            }
            break;
        case WorldGeneration::BLOCKED:
            for(Tile &t : _tiles) {
                t._type = Tile::STONE;
            }
            break;
    }
}
