#pragma once

#include "item.h"
#include "arena_allocator.h"

class Robot;

typedef short int TileData;

class Tile {
private:
    Item type = Item::EMPTY;
    TileData data = 0;
public:
    ArenaPointer<Robot> robot;
    Tile(Item type, short int data);
    Tile(Item type);
    Tile(){}
    bool operator==(const Tile& tile) const&;
    bool empty() const&;
    Item get_type() const&;

    //box functions
    Item get_box_item() const&;
    short int get_box_item_cnt() const&;
    Item remove_box_item();
    bool add_box_item(Item item);
    //apple functions
    bool is_apple_grow() const&;
    void set_apple_grow(bool grow);
    //fire functions
    bool is_fire_charcoal() const&;
    void set_fire_charcoal(bool charcoal);
    
    void draw(int x, int y) const&;
};

