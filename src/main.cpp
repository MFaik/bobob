#include "game.h"

extern Game g_game;

int main() {
    g_game.setup_code({
            0b10000101111111111111111111111111,//GO
            0b10000011111111100000000000000001,//TURN EAST
            0b10000101111111111111111111111111,//GO
            0b00000000000000000000000000000101,//MOV A RIGHT
            0b10000011111111110000000000000000,//TURN A
            0b10000101111111111111111111111111,//GO
            }, {});


    g_game.add_robot(0, 0);
    Robot* robot = g_game.get_robot(0, 0);
    g_game.draw();
    //TODO: add the robot to chunk and tick the chunks 
    for(size_t i = 0;i < 6;i++) {
        robot->tick();   
        g_game.draw();
    }
}
