#include "map_elements.h"

void draw(Tile tile) {
    std::cout << tile_char[tile];
}
void draw(Object obj) {
    std::cout << obj_char[obj];
}
