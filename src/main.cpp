#include "game.h"
#include "code_runner.h"

extern Game g_game;
extern CodeRunner g_code_runner;

int main() {
    g_game.setup(6, 5);
    g_code_runner.setup({
            0b10000101111111111111111111111111,//GO
            0b10000011111111100000000000000001,//TURN EAST
            0b10000101111111111111111111111111,//GO
            0b00000000000000000000000000000101,//MOV A RIGHT
            0b10000011111111110000000000000000,//TURN A
            0b10000101111111111111111111111111,//GO
            }, {});


    g_game.add_robot(3, 4);
    Robot* robot = g_game.get_robot(3, 4);
    g_game.draw();
    for(size_t i = 0;i < 6;i++) {
        robot->tick();   
        g_game.draw();
    }
}
