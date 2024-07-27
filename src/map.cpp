#include <unordered_set>

#include "map.h"
#include "robot.h"

Map g_map;

Color Map::get_tile_color(Tile::Type tile_type) {
    switch (tile_type) {
        case Tile::GRASS:
            return GREEN;
        case Tile::STONE:
            return DARKGRAY;
        case Tile::PATH:
            return DARKBROWN;
        case Tile::TREE:
            return DARKGREEN;
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
    auto chunk = _chunks.find({chunk_x, chunk_y});
    if(chunk == _chunks.end()) {
        _chunks[{chunk_x, chunk_y}] = (Chunk*)calloc(1, sizeof(Chunk));
    }
}

//TODO: we can remove the conditional if the chunk is guaranteed to exist
void Map::unload_chunk(int chunk_x, int chunk_y) {
    auto chunk = _chunks.find({chunk_x, chunk_y});
    if(chunk != _chunks.end()) {
        for(auto robot : chunk->second->_robots) {
            free(robot);
        }
        free(chunk->second);
        _chunks.erase(chunk);
    }
}

void Map::clean_chunks() {
    for(auto &chunk : _chunks) {
        for(auto robot : chunk.second->_robots) {
            free(robot);
        }
        free(chunk.second);
    }
    _chunks.clear();
}

inline int Map::global_to_local(int x, int y) {
    return ((x%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE)*CHUNK_SIZE +
        ((y%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE);
}

//TODO: we can remove the conditional if the chunk is guaranteed to exist
Chunk& Map::get_chunk(int x, int y) {
    auto chunk = _chunks.find({x/CHUNK_SIZE, y/CHUNK_SIZE});
    if(chunk == _chunks.end()) {
        load_chunk(x/CHUNK_SIZE, y/CHUNK_SIZE);
        return *_chunks[{x/CHUNK_SIZE, y/CHUNK_SIZE}];
    }
    return *chunk->second;
}

Tile Map::get_tile(int x, int y) {
    return get_chunk(x, y)._tiles[global_to_local(x, y)];
}

void Map::set_tile(int x, int y, Tile tile) {
    get_chunk(x, y)._tiles[global_to_local(x, y)] = tile;
}

void Map::add_robot(Robot *robot) {
    get_chunk(robot->_x, robot->_y)._robots.push_back(robot);
}

Robot* Map::get_robot(int x, int y) {
    for(auto robot : get_chunk(x, y)._robots) {
        if(robot->_x == x && robot->_y == y) {
            return robot;
        }
    }
    return nullptr;
}

void Map::remove_robot(int x, int y) {
    auto &chunk = get_chunk(x, y);
    for(auto it = chunk._robots.begin();it != chunk._robots.end();it++) {
        if((*it)->_x == x && (*it)->_y == y) {
            free(*it);
            chunk._robots.erase(it);
            return;
        }
    }
}

//TODO: load and unload chunks depending on:
//their robot count
//neighbour robot count
//currently this is a memory leak
void Map::move_robot(Chunk &last_chunk, Robot *robot) {
    for(auto it = last_chunk._robots.begin();it != last_chunk._robots.end();it++) {
        if(*it == robot) {
            add_robot(*it);
            last_chunk._robots.erase(it);
            return;
        }
    }
}

void Map::tick() {
    //TODO: this can cause problems
    //if we remove a robot
    //then add a new robot
    //and the pointer happens to be the same
    //this completely blows up
    //TODO: profile this code
    std::unordered_set<Robot*> no_update_list;
    no_update_list.clear();
    for(auto chunk : _chunks) {
        std::vector<Robot*> moved_robots;
        for(auto &robot : chunk.second->_robots) {
            if(no_update_list.find(robot) != no_update_list.end()) {
                continue;
            }
            robot->tick();
            if(robot->_x/CHUNK_SIZE != chunk.first[0] ||
               robot->_y/CHUNK_SIZE != chunk.first[1]) {
                moved_robots.push_back(robot);
                no_update_list.insert(robot);
            }
        }
        for(auto robot : moved_robots)
            move_robot(*chunk.second, robot);
    }
}
