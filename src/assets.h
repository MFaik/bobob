#include <raylib.h>

#include <unordered_map>
#include <vector>

#include "item.h"

class Assets {
    std::unordered_map<Item, std::vector<Texture2D>> _textures;
    void load_texture(Item item, size_t cnt);
public:
    void load_textures();
    bool is_textures_loaded();

    const Texture2D& get_texture(Item item);
    const Texture2D& get_texture(Item item, unsigned int variation);
};

