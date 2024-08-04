#include "map.h"
#include "robot.h"
#include "game.h"

extern Game g_game;

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

void Map::load_chunk(int chunk_x, int chunk_y) {
    //TODO: this will probably have some async file read
    //and/or procedural generation thing
    
}

//TODO: we can remove the conditional if the chunk is guaranteed to exist
void Map::unload_chunk(int chunk_x, int chunk_y) {
    auto chunk = _chunks.find({chunk_x, chunk_y});
    if(chunk != _chunks.end()) {
        _chunks.erase(chunk);
    }
}

void Map::clean_chunks() {
    _chunks.clear();
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
        load_chunk(x/CHUNK_SIZE, y/CHUNK_SIZE);
        return _chunks[{x/CHUNK_SIZE, y/CHUNK_SIZE}];
    }
    return chunk->second;
}

Tile& Map::get_tile(int x, int y) {
    return get_chunk(x, y)._tiles[global_to_local(x, y)];
}

void Map::add_robot(int x, int y, ArenaPointer<Robot> robot) {
    get_tile(x, y)._robot = robot;
}

void Map::remove_robot(int x, int y) {
    get_tile(x, y)._robot = ArenaPointer<Robot>();
}

ArenaPointer<Robot> Map::get_robot(int x, int y) {
    return get_tile(x, y)._robot;
}

void Map::tick() {

}
