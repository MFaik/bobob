#include <chrono>

#include <raylib.h>

#include "game.h"
#include "program_parser.h"
extern Game g_game;

namespace sc = std::chrono;

int main() {
    auto prog = parse_program("SEL 2\nMOV A 0\nlabel0:\nUSE\nGO\nADD A 1\nCMP A 10\n JNE label0\nTURN RIGHT");
    g_game.setup_program(prog);

    for(auto e : prog._errors) {
        std::cout << e.txt << std::endl;
    }

    if(prog._errors.size()) {
        return 1;
    }

    InitWindow(600, 600, "bobob");

    g_game.set_tile(0, 0, {Tile::IRON_MINE, 0});
    g_game.set_tile(0, 4, {Tile::IRON_MINE, 0});

    std::array<Item, 16> inv{};
    inv[2] = PATH_ITEM;
    
    for(int y = 11;y < 500;y++) {
        for(int x = 1;x < 500;x++) {
            g_game.add_robot(x, y, inv);
        }
    }
    
    auto last_update = sc::steady_clock::now();
    while(!WindowShouldClose()) {
        auto now = sc::steady_clock::now();
        if(now-last_update > sc::milliseconds{1000/20}) {
            g_game.tick();

            last_update = now;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        g_game.draw();

        EndDrawing();
    }
}
