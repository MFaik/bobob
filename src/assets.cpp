#include "assets.h"


#include <format>

Assets g_assets;

bool Assets::is_textures_loaded() {
    for(auto [_, list] : _textures) {
        for(auto texture : list) {
            if(!IsTextureReady(texture)) {
                return false;
            }
        }
    }
    return true;
}

//TODO: make this data driven
void Assets::load_textures() {
    load_texture(Item::APPLE, 3);
    load_texture(Item::STONE, 3);
    load_texture(Item::CHARCOAL, 1);
    load_texture(Item::BOX, 1);
    load_texture(Item::FIRE, 2);
    load_texture(Item::EMPTY, 1);
    load_texture(Item::PATH, 1);
    load_texture(Item::ROBOT, 1);
    load_texture(Item::LAVA, 1);
}

void Assets::load_texture(Item item, size_t cnt) {
    for(size_t i = 0;i < cnt;i++) {
        const auto& name = std::format("assets/{}{}.png", get_item_name(item), i);
        _textures[item].push_back(LoadTexture(name.c_str()));
    }
}

const Texture2D& Assets::get_texture(Item item) {
    return get_texture(item , 0);
}

const Texture2D& Assets::get_texture(Item item, unsigned int variation) {
    const auto& texture_list = _textures[item];
    return texture_list[variation%texture_list.size()];
}
