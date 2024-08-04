#include <chrono>

#include <raylib.h>

#include "game.h"
#include "program_parser.h"
extern Game g_game;

namespace sc = std::chrono;

int main() {
    auto prog = parse_program("SEL 2\nMOV A 0\nloop:\nUSE\ngo_retry:\nGO\nCMP INPUT 999\nJEQ go_retry\nADD A 1\nCMP A 50\n JNE loop\nTURN RIGHT");
    g_game.setup_program(prog);

    for(auto e : prog._errors) {
        std::cout << e.txt << std::endl;
    }

    if(prog._errors.size()) {
        return 1;
    }


    g_game.get_tile(0, 0)._type = Tile::IRON_MINE;
    g_game.get_tile(0, 4)._type = Tile::IRON_MINE;

    std::array<Item, 16> inv{};
    inv[2] = PATH_ITEM;
    
    for(int y = 10;y < 200;y++) {
        for(int x = 10;x < 200;x++) {
            g_game.add_robot(x, y, inv);
        }
    }

    InitWindow(GetScreenWidth(), GetScreenHeight(), "bobob");
    SetTargetFPS(60);
    
    while(!WindowShouldClose()) {
        BeginDrawing();
        g_game.tick();
        //TODO: make this actually fixed update
        g_game.fixed_tick();

        ClearBackground(RAYWHITE);
        g_game.draw();
        
        DrawFPS(0, 0);
        EndDrawing();
    }
}
