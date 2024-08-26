#include <algorithm>
#include <raylib.h>

#include "map.h"
#include "robot.h"
#include "game.h"

template class ArenaAllocator<Chunk>;

//TODO: fix 50 magic number
//TODO: fix drawing
void Map::draw(int x, int y) {
    get_tile(x, y).draw(x, y, _game);
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
    _chunk_allocator.get(chunk)->generate(chunk_x, chunk_y, _world_generation);
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

int Map::global_to_local(int x, int y) const& {
    return ((x%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE) +
        ((y%CHUNK_SIZE+CHUNK_SIZE)%CHUNK_SIZE)*CHUNK_SIZE;
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
    return get_chunk(x, y).tiles[global_to_local(x, y)];
}

const Tile& Map::get_tile(int x, int y) {
    return get_chunk(x, y).tiles[global_to_local(x, y)];
}

Tile& Map::get_tile_ref(int x, int y) {
    Chunk& chunk = get_chunk(x, y);
    chunk.edited = true;
    return chunk.tiles[global_to_local(x, y)];
}

void Map::add_robot(int x, int y, ArenaPointer<Robot> robot) {
    get_tile_internal(x, y).robot = robot;
}

void Map::remove_robot(int x, int y) {
    get_tile_internal(x, y).robot = ArenaPointer<Robot>();
}

ArenaPointer<Robot> Map::get_robot(int x, int y) {
    return get_tile(x, y).robot;
}
//TODO: remove repeated get_tile calls
//TODO: make this code better?
Item Map::use(int x, int y, Item item, bool manual) {
    const Tile& tile = get_tile(x, y);
    switch(item) {
        case Item::EMPTY:
            switch (tile.get_type()) {
                case Item::BOX:
                    return get_tile_ref(x, y).remove_box_item();
                case Item::APPLE:
                    if(tile.is_apple_grow())
                        get_tile_ref(x, y).set_apple_grow(0);
                    else
                        get_tile_ref(x, y) = Tile(Item::EMPTY);
                    return Item::APPLE;
                //non pickable items
                case Item::FIRE:
                case Item::LAVA:
                case Item::ROBOT:
                case Item::BASE:
                case Item::PATH:
                case Item::EMPTY:
                    break;
                //items that don't require extra code to pick up
                case Item::STONE:
                case Item::CHARCOAL:
                    Item type = tile.get_type();
                    get_tile_ref(x, y) = Tile(Item::EMPTY);
                    return type;
            }
            //can't put empty in box
            return Item::EMPTY;
        case Item::STONE:
            if(tile.get_type() == Item::EMPTY) {
                get_tile_ref(x, y) = Tile(item);
                return Item::EMPTY;
            }
            if(tile.get_type() == Item::CHARCOAL) {
                push_update({x, y, TileUpdate::BURN}, manual);
                return Item::EMPTY;
            }
            break;
        case Item::LAVA:
            if(tile.get_type() == Item::EMPTY) {
                get_tile_ref(x, y) = Tile(item);
                push_update({x, y, TileUpdate::BURN});
            }
            break;
        //placable items
        case Item::CHARCOAL:
        case Item::PATH:
            if(tile.get_type() == Item::EMPTY) {
                get_tile_ref(x, y) = Tile(item);
                return Item::EMPTY;
            }
            break;
        case Item::BOX:
            if(tile.get_type() == Item::EMPTY) {
                get_tile_ref(x, y) = Tile(Item::BOX);
                return Item::EMPTY;
            }
            //can't put boxes in boxes
            return item;
        case Item::APPLE:
            if(tile.get_type() == Item::EMPTY) {
                get_tile_ref(x, y) = Tile(item);
                push_update({x, y, TileUpdate::APPLE}, manual);
                return Item::EMPTY;
            }
            break;
        case Item::FIRE:
            if(tile.get_type() == Item::APPLE) {
                //this keeps the apple data and passes it to fire
                //thus data.apple_growth -> data.fire_charcoal
                push_update({x, y, TileUpdate::BURN}, manual);
            }
            break;
        case Item::ROBOT:
            if(tile.robot.empty()) {
                _game.add_robot(x, y);
                return Item::EMPTY;
            }
            return Item::ROBOT;
        //unusable items
        case Item::BASE:
            return Item::EMPTY;
    }
    
    if(tile.get_type() == Item::BOX) {
        if(get_tile_ref(x, y).add_box_item(item))
            return Item::EMPTY;
    }
    if(tile.get_type() == Item::BASE) {
        if(_game.add_item_to_base(item))
            return Item::EMPTY;
    }

    return item;
}

void Map::push_update(TileUpdate update, bool first) {
    if(_game.is_paused() && first) {
        update.update(*this);
    } else {
        _tile_updates.push_back(update);
    }
}

void Map::tick() {
    if(_tile_updates.size() == 0)
        return;
    std::vector<TileUpdate> curr(_tile_updates);
    _tile_updates.clear();
    // TODO: move this to its own function?
    // make sure to change tileupdate x y and type to private
    // lower priority updates are cancelled
    auto compare = [](TileUpdate l, TileUpdate r) {
        if(l.x != r.x)
            return l.x < r.x;
        if(l.y != r.y)
            return l.y < r.y;
        return l.type < r.type;
    };
    std::sort(curr.begin(), curr.end(), compare);
    auto eq = [](TileUpdate l, TileUpdate r) {
        return l.x == r.x && l.y == r.y;
    };
    auto last = std::unique(curr.begin(), curr.end(), eq);

    for(auto it = curr.begin();it != last;it++){ 
        it->update(*this);
    }
}


void Chunk::generate(int chunk_x, int chunk_y, WorldGeneration world_gen) {
    edited = false;
    switch(world_gen) {
        case WorldGeneration::EMPTY:
            break;
        case WorldGeneration::BLOCKED:
            for(Tile &t : tiles) {
                t = Tile(Item::STONE);
            }
            break;
    }
    if(chunk_x == 0 && chunk_y == 0) {
        for(int y = 0;y < 3;y++) {
            for(int x = 0;x < 3;x++) {
                tiles[x+y*CHUNK_SIZE] = Tile(Item::BASE, x+y*3);
            }
        }
    }
}
