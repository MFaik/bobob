#include <algorithm>
#include <raylib.h>

#include "map.h"
#include "robot.h"

#include "game.h"
extern Game g_game;

#include "assets.h"
extern Assets g_assets;

template class ArenaAllocator<Chunk>;

//TODO: remove this
constexpr short int BOX = (short int)(1<<15);

bool Tile::operator==(const Tile& tile) const& {
    return tile.type == type && tile.data == data;
}
bool Tile::empty() const& {
    return type != Item::EMPTY && data;
}
Item Tile::get_type() const& {
    if(data&BOX)
        return Item::BOX;
    else
        return type;
}
short int Tile::get_item_cnt() const& {
    return data&(~BOX);
}

//TODO: fix 50 magic number
//TODO: fix drawing
void Map::draw(int x, int y) {
    const Tile& tile = get_tile(x, y);
    switch (tile.get_type()) {
        case Item::EMPTY:
        case Item::ROBOT:
            break;
        case Item::FIRE:
            DrawTexturePro(
                g_assets.get_texture(Item::FIRE, tile.data),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            break;
        case Item::APPLE:
            DrawTexturePro(
                g_assets.get_texture(Item::APPLE, tile.data+1),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            break;
        case Item::BOX:
            DrawTexturePro(
                g_assets.get_texture(Item::BOX, 0),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            if(tile.get_item_cnt())
                DrawTexturePro(
                    g_assets.get_texture(tile.type, 0),
                    Rectangle{0, 0, 50, 50}, 
                    Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                               (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                               TILE_SIZE, TILE_SIZE }, 
                    Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
                );
            DrawText(std::to_string(tile.get_item_cnt()).c_str(), 
                    x*TILE_SIZE+TILE_SIZE*0.6f, y*TILE_SIZE+TILE_SIZE*0.5f, 
                    12, WHITE);
            break;
        case Item::CHARCOAL:
        case Item::PATH:
        case Item::STONE:
        case Item::LAVA:
            //TODO: add noise
            DrawTexturePro(
                g_assets.get_texture(tile.get_type(), x+y),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            break;
    }
    if(!tile.robot.empty()) {
        DrawTexturePro(
            g_assets.get_texture(Item::ROBOT, 0),
            Rectangle{0, 0, 50, 50}, 
            Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                       (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                       TILE_SIZE, TILE_SIZE }, 
            Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 
            g_game.get_robot(tile.robot)->_dir*90, WHITE
        );
    }
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
            if(tile.get_type() == Item::BOX) {
                if(tile.get_item_cnt() == 0) {
                    get_tile_ref(x, y) = Tile(Item::EMPTY);
                    return Item::BOX;
                }
                Tile &t = get_tile_ref(x, y);
                t.data--;
                Item ret = t.type;
                if(t.get_item_cnt() == 0) {
                    t.type = Item::EMPTY;
                }
                return ret;
            }
            if(tile.get_type() == Item::APPLE) {
                if(tile.data)
                    get_tile_ref(x, y).data = 0;
                else
                    get_tile_ref(x, y) = Tile(Item::EMPTY);
                return Item::APPLE;
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
                get_tile_ref(x, y).data = BOX;
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
                g_game.add_robot(x, y);
                return Item::EMPTY;
            }
            return Item::ROBOT;
    }
    
    if(tile.get_type() == Item::BOX) {
        if(tile.type == Item::EMPTY) {
            Tile& t = get_tile_ref(x, y);
            t.type = item;
            t.data++;
        } else if(tile.type == item) {
            Tile& t = get_tile_ref(x, y);
            t.data++;
        }
        return Item::EMPTY;
    }
    return item;
}

void Map::push_update(TileUpdate update, bool first) {
    if(g_game.is_paused() && first) {
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


void Chunk::generate(WorldGeneration world_gen) {
    edited = false;
    switch(world_gen) {
        case WorldGeneration::EMPTY:
            break;
        case WorldGeneration::BLOCKED:
            for(Tile &t : tiles) {
                t.type = Item::STONE;
            }
            break;
    }
}
