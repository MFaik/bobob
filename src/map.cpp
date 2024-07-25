#include "map.h"

Map g_map;

void Map::draw(Tile::Type tile_type) {
    switch (tile_type) {
        case Tile::GRASS:
            std::cout << ',';
            break;
        case Tile::STONE:
            std::cout << '.';
            break;
        case Tile::PATH:
            std::cout << 'o';
            break;
        case Tile::TREE:
            std::cout << 'T';
            break;
        case Tile::COAL_MINE:
            std::cout << 'c';
            break;
        case Tile::IRON_MINE:
            std::cout << 'i';
            break;
        default:
            std::cout << '?';
    }
}

void Map::draw(int x, int y) {
    draw(get_tile(x, y)._type);
}

void Map::load_chunk(int x, int y) {
    //TODO: this will probably have some async file
    //and/or procedural generation thing
    auto chunk = _chunks.find({x, y});
    if(chunk == _chunks.end()) {
        _chunks[{x, y}] = (Chunk*)calloc(1, sizeof(Chunk));
    }
}

void Map::unload_chunk(int x, int y) {
    _chunks.erase({x, y});
}

void Map::clean_chunks() {
    _chunks.clear();
}

inline int Map::convert_position(int x, int y) {
    return ((x%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE)*CHUNK_SIZE +
           ((y%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE);
}

//TODO: find a way to eliminate the conditional
Tile Map::get_tile(int x, int y) {
    auto chunk = _chunks.find({x/32, y/32});
    if(chunk == _chunks.end()) {
        load_chunk(x/32, y/32);
        return _chunks[{x/32, y/32}]->_tiles[convert_position(x, y)];
    } else {
        return chunk->second->_tiles[convert_position(x, y)];
    }
}

void Map::set_tile(int x, int y, Tile tile) {
    auto chunk = _chunks.find({x/32, y/32});
    if(chunk == _chunks.end()) {
        load_chunk(x/32, y/32);
        _chunks[{x/32, y/32}]->_tiles[convert_position(x, y)] = tile;
    } else {
        chunk->second->_tiles[convert_position(x, y)] = tile;
    }
}
