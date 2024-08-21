#pragma once

#include <unordered_map>

enum class Item : short int {
    EMPTY,
    STONE,
    PATH,
    APPLE,
    FIRE,
    CHARCOAL,
    //these items are not used in tiles
    ROBOT,
    BOX,
};

//TODO: fix this
#ifdef ITEM_NAME_IMPLEMENTATION
const std::unordered_map<Item, const char*>& get_item_name_map() {
    static const std::unordered_map<Item, const char*> item_name_map = {
        {Item::EMPTY, "EMPTY"},
        {Item::STONE, "STONE"},
        {Item::PATH, "PATH"},
        {Item::APPLE, "APPLE"},
        {Item::FIRE, "FIRE"},
        {Item::CHARCOAL, "CHARCOAL"},
        {Item::ROBOT, "ROBOT"},
        {Item::BOX, "BOX"},
    };
    return item_name_map;
}
#else
const std::unordered_map<Item, const char*>& get_item_name_map();
#endif

const char* get_item_name(Item item);

