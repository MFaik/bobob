#include "catch_amalgamated.h"

#include "game.h"
#include "program_parser.h"

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
        "MOV A 12\
        MOV COND 32\
        MOV B 20\
        MOV C 18\
        JMP loop\
        loop:\
        GO\
        SUB COND 1\
        JNE loop\
        JEQ skip\
        skip:\
        LOOK\
        NOP\
        CMP A 12\
        USE C\
        TURN BACK\
        TURN SOUTH\
        ADD A B\
        ADD C 3";
    Program program = parse_program(program_str);
    game.setup_program(program);

    Game game2;
    game2.load_game(game.save_game());

    SECTION("robots") {
        for(int i = -20;i < 20;i++) {
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

