#define ITEM_NAME_IMPLEMENTATION
#include "item.h"

const char* get_item_name(Item item) {
    return get_item_name_map().at(item);
}
