#include <chrono>

#include <raylib.h>

#include "game.h"
extern Game g_game;

namespace sc = std::chrono;

int main() {
    InitWindow(600, 600, "bobob");

    std::vector<unsigned> code = {
        0b10001011111111100000000000000010,//SEL 2
        0b00000000000000000000000000000000,//MOV a 0
        //LABEL0
        0b10001001111111111111111111111111,//USE
        0b10000101111111111111111111111111,//GO
        0b00000010000000000000000000000001,//ADD a 1
        0b00010110000000000000000000001010,//COMP a 10
        0b00011101111111100000000000000000,//JNE 0
        0b10000011111111100000000000000101,//TURN RIGHT
    };
    std::vector<unsigned> labels = {
        2,
    };
    g_game.setup_code(code, labels);

    g_game.set_tile(0, 0, {Tile::IRON_MINE, 0});
    g_game.set_tile(0, 4, {Tile::IRON_MINE, 0});

    g_game.add_robot(20, 20);
    Robot *robot = g_game.get_robot(20, 20);
    robot->_inventory[2] = PATH_ITEM;
    g_game.add_robot(25, 20);
    robot = g_game.get_robot(25, 20);
    robot->_inventory[2] = PATH_ITEM;
    g_game.add_robot(20, 25);
    robot = g_game.get_robot(20, 25);
    robot->_inventory[2] = PATH_ITEM;
    
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
