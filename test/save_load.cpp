#include "catch_amalgamated.h"

#include <string>

#include "game.h"
#include "robot.h"
#include "tile.h"
#include "program_parser.h"
#include "program.h"

TEST_CASE("game save", "[game]") {
    Game game;

    game.use(-1327, -2149, Item::APPLE);
    for(int i = -100;i < 100;i++) {
        game.use(i*131, i*-28, Item::STONE);
        game.use(i*-183, i*-91, Item::STONE);
    }

    for(int i = -20;i < 20;i++) {
        game.add_robot(i*100+12, i*20+17);
        Robot* robot = game.get_robot(i*100+12, i*20+17);
        robot->_a = i*2;
        robot->_b = i*3;
        robot->_c = i*4;
        robot->_dir = Direction::SOUTH;
        robot->_pc = i;
        robot->_cond = i*10;
    }

    std::string program_str = 
        "MOV A 12\n\
        MOV COND 32\n\
        MOV B 20\n\
        MOV C 18\n\
        JMP loop\n\
        loop:\n\
        GO\n\
        SUB COND 1\n\
        JNE loop\n\
        JEQ skip\n\
        skip:\n\
        LOOK\n\
        NOP\n\
        CMP A 12\n\
        USE C\n\
        TURN BACK\n\
        TURN SOUTH\n\
        ADD A B\n\
        ADD C 3\n";
    Program program = parse_program(program_str);
    game.set_program(program);

    Game game2;
    game2.load_game(game.save_game());

    SECTION("robots") {
        for(int i = -20;i < 20;i++) {
            if(game.get_tile(i*100+12, i*20+17).get_type() == Item::ROBOT)
                REQUIRE(*game.get_robot(i*100+12, i*20+17) == 
                        *game2.get_robot(i*100+12, i*20+17));
        }
    }

    SECTION("tiles") {
        REQUIRE(game.get_tile(-1327, -2149) == game2.get_tile(-1327, -2149));
        for(int i = -100;i < 100;i++) {
            REQUIRE(game.get_tile(i*131, i*-28) == game2.get_tile(i*131, i*-28));
            REQUIRE(game.get_tile(i*-183, i*-91) == game2.get_tile(i*-183, i*-91));
        }
    }

    SECTION("program") {
        REQUIRE(program == parse_program(game2.get_program()));
    }
}

