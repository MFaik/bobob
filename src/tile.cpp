#include "tile.h"

#include "item.h"
#include "game.h"

#include "assets.h"
extern Assets g_assets;

//TODO: remove this
constexpr short int BOX = (short int)(1<<15);

Tile::Tile(Item type): type(type) {
    if(type == Item::BOX) {
        this->type = Item::EMPTY;
        data = BOX;
    }
}

Tile::Tile(Item type, short int data) : type(type), data(data) {
    if(type == Item::BOX) {
        this->type = Item::EMPTY;
        this->data = BOX|data;
    }
}

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

Item Tile::get_raw_type() const& {
    return type;
}

TileData Tile::get_raw_data() const& {
    return data;
}

//this function assumes tile is a box
Item Tile::get_box_item() const& {
    return type;
}

//this function assumes tile is a box
short int Tile::get_box_item_cnt() const& {
    return data&(~BOX);
}

//this function assumes tile is a box
Item Tile::remove_box_item() {
    if(get_box_item_cnt() == 0) {
        //this is assumed
        //type = Item::EMPTY
        data = 0;
        return Item::BOX;
    }
    data--;
    Item ret = type;
    if(get_box_item_cnt() == 0) {
        type = Item::EMPTY;
    }
    return ret;
}

//this function assumes tile is a box
bool Tile::add_box_item(Item item) {
    if(get_box_item() == Item::EMPTY) {
        type = item;
        data++;
        return true;
    } else if(type == item) {
        data++;
        return true;
    }
    return false;
}

//this function assumes tile is apple
bool Tile::is_apple_grow() const& {
    return data;
}

//this function assumes tile is apple
void Tile::set_apple_grow(bool grow) {
    data = grow;
}

//this function assumes tile is fire
bool Tile::is_fire_charcoal() const& {
    return data;
}

void Tile::set_fire_charcoal(bool charcoal) {
    data = charcoal;
}

void Tile::draw(int x, int y, Game& game) const& {
    switch (get_type()) {
        case Item::EMPTY:
        case Item::ROBOT:
            break;
        //data = texture_variation
        case Item::BASE:
        case Item::FIRE:
            DrawTexturePro(
                g_assets.get_texture(get_type(), data),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            break;
        case Item::APPLE:
            DrawTexturePro(
                g_assets.get_texture(Item::APPLE, data+1),
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
            if(get_box_item_cnt())
                DrawTexturePro(
                    g_assets.get_texture(get_box_item(), 0),
                    Rectangle{0, 0, 50, 50}, 
                    Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                               (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                               TILE_SIZE, TILE_SIZE }, 
                    Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
                );
            DrawText(std::to_string(get_box_item_cnt()).c_str(), 
                    x*TILE_SIZE+TILE_SIZE*0.6f, y*TILE_SIZE+TILE_SIZE*0.5f, 
                    30, WHITE);
            break;
        case Item::CHARCOAL:
        case Item::PATH:
        case Item::STONE:
        case Item::LAVA:
            //TODO: add noise
            DrawTexturePro(
                g_assets.get_texture(get_type(), x+y),
                Rectangle{0, 0, 50, 50}, 
                Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                           (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                           TILE_SIZE, TILE_SIZE }, 
                Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 0, WHITE
            );
            break;
    }
    if(!robot.empty()) {
        DrawTexturePro(
            g_assets.get_texture(Item::ROBOT, 0),
            Rectangle{0, 0, 50, 50}, 
            Rectangle{ (float)x*TILE_SIZE+TILE_SIZE/2.0f, 
                       (float)y*TILE_SIZE+TILE_SIZE/2.0f, 
                       TILE_SIZE, TILE_SIZE }, 
            Vector2{TILE_SIZE/2.0, TILE_SIZE/2.0}, 
            game.get_robot(robot)->_dir*90, WHITE
        );
    }
}
