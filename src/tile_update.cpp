#include <array>

#include "tile_update.h"
#include "map.h"
#include "item.h"

struct offset {
    char x;
    char y;
};
constexpr std::array<offset, 4> d_cross = { offset{1, 0}, {-1, 0}, {0, 1}, {0, -1} };
void TileUpdate::push_update_around(Map& map, Type type) {
    switch(type) {
        case TileUpdate::BURN:
            map.push_update({x, y, TileUpdate::BURN});
            for(auto d : d_cross) {
                auto tile_type = map.get_tile(x+d.x, y+d.y).get_type();
                //these are flammable items rather than items that can burn
                if(tile_type == Item::APPLE || tile_type == Item::STONE)
                    map.push_update({x+d.x, y+d.y, TileUpdate::BURN});
            }
            break;
        case TileUpdate::FLOW:
            for(auto d : d_cross) {
                auto tile = map.get_tile(x+d.x, y+d.y).get_type();
                if(tile == Item::EMPTY)
                    map.push_update({x+d.x, y+d.y, TileUpdate::FLOW});
            }
            break;
        //these updates can't be pushed around
        case TileUpdate::APPLE:
            break;
    }
}

void TileUpdate::update(Map& map) {
    switch(type) {
        case APPLE:
            update_apple(map);
            break;
        case BURN:
            update_burn(map);
            break;
        case FLOW:
            update_flow(map);
            break;
    }
}

void TileUpdate::update_apple(Map& map) {
    int cnt = 0;
    cnt += map.get_tile(x+1, y).get_type() == Item::APPLE;
    cnt += map.get_tile(x-1, y).get_type() == Item::APPLE;
    cnt += map.get_tile(x, y+1).get_type() == Item::APPLE;
    cnt += map.get_tile(x, y-1).get_type() == Item::APPLE;
    if(cnt >= 3) {
        map.get_tile_ref(x, y).data = 1;
    }
}

void TileUpdate::update_burn(Map& map) {
    const Tile& tile = map.get_tile(x, y);
    switch(tile.get_type()) {
        case Item::FIRE:
            if(tile.data) {
                map.get_tile_ref(x, y) = Tile(Item::CHARCOAL);
            } else {
                map.get_tile_ref(x, y) = Tile(Item::EMPTY);
            }
            break;
        case Item::APPLE:
            //TODO: remove this
            //fire data 0 -> empty, 1 -> charcoal
            //this is the same as apple growth 
            //thus just assigning the type is fine
            map.get_tile_ref(x, y).type = Item::FIRE;
            push_update_around(map, BURN);
            break;
        case Item::STONE:
            map.get_tile_ref(x, y) = Tile(Item::LAVA);       
            map.push_update({x, y, FLOW});
            break;
        case Item::LAVA:
            map.get_tile_ref(x, y) = Tile(Item::STONE);
            break;
        //these are items that just burn and dissapear
        case Item::CHARCOAL:
            map.get_tile_ref(x, y) = Tile(Item::FIRE);
            map.push_update({x, y, BURN});
            push_update_around(map, BURN);
            break;
        //these are items that can't burn
        case Item::EMPTY:
        case Item::ROBOT:
        case Item::BOX:
        case Item::PATH:
            break;
    }
}

void TileUpdate::update_flow(Map& map) {
    Item type = map.get_tile(x, y).get_type();
    if(type == Item::EMPTY) {
        map.get_tile_ref(x, y) = Tile(Item::LAVA);
        map.push_update({x, y, BURN});
    } else if(type == Item::LAVA) {
        map.push_update({x, y, BURN});
        push_update_around(map, FLOW);
    }
}
