#include "catch_amalgamated.h"

#include "game.h"
#include "robot.h"
#include "program_parser.h"

TEST_CASE("robot", "[robot]") {
    Game game;
    SECTION("look") {
        game.add_robot(10, 11);   
        Robot* look_stone1 = game.get_robot(10, 11);
        look_stone1->_dir = Direction::NORTH;
        game.add_robot(10, 9);   
        Robot* look_stone2 = game.get_robot(10, 9);
        look_stone2->_dir = Direction::SOUTH;
        game.add_robot(9, 10);   
        Robot* look_stone3 = game.get_robot(9, 10);
        look_stone3->_dir = Direction::EAST;
        game.add_robot(11, 10);   
        Robot* look_stone4 = game.get_robot(11, 10);
        look_stone4->_dir = Direction::WEST;
        game.use(10, 10, Item::STONE);

        game.add_robot(31, 10);
        Robot* look_apple = game.get_robot(31, 10);
        game.use(31, 9, Item::APPLE);

        game.add_robot(32, 10);
        Robot* look_box = game.get_robot(32, 10);
        game.use(32, 9, Item::BOX);

        game.add_robot(33, 10);
        Robot* look_charcoal = game.get_robot(33, 10);
        game.use(33, 9, Item::CHARCOAL);

        game.set_program(parse_program("LOOK"));
        game.fixed_tick();

        REQUIRE(look_stone1->_input == (RegisterData)Item::STONE);
        REQUIRE(look_stone2->_input == (RegisterData)Item::STONE);
        REQUIRE(look_stone3->_input == (RegisterData)Item::STONE);
        REQUIRE(look_stone4->_input == (RegisterData)Item::STONE);
        REQUIRE(look_apple->_input == (RegisterData)Item::APPLE);
        REQUIRE(look_box->_input == (RegisterData)Item::BOX);
        REQUIRE(look_charcoal->_input == (RegisterData)Item::CHARCOAL);
    }

    SECTION("go turn") {
        game.add_robot(20, 100);
        Robot* robot1 = game.get_robot(20, 100);
        game.add_robot(-200, 120);
        Robot* robot2 = game.get_robot(-200, 120);

        game.set_program(parse_program("GO\n\
                                        TURN RIGHT\n\
                                        GO\n\
                                        TURN BACK\n\
                                        GO\n\
                                        TURN LEFT\n\
                                        GO\n\
                                        TURN NORTH\n\
                                        GO\n\
                                        TURN SOUTH\n\
                                        GO\n\
                                        TURN EAST\n\
                                        GO\n\
                                        TURN WEST\n\
                                        GO\n\
                                        GO\n\
                                        GO\n\
                                        GO\n\
                                        GO"));
        //GO
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 99);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 119);
        //TURN RIGHT
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 21);REQUIRE(robot1->_y == 99);
        REQUIRE(robot2->_x == -199);REQUIRE(robot2->_y == 119);
        //TURN BACK
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 99);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 119);
        //TURN LEFT
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 120);
        //TURN NORTH
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 99);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 119);
        //TURN SOUTH
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 120);
        //TURN EAST
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 21);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -199);REQUIRE(robot2->_y == 120);
        //TURN WEST
        //GO
        game.fixed_tick();
        game.fixed_tick();
        REQUIRE(robot1->_x == 20);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -200);REQUIRE(robot2->_y == 120);
        //GO
        game.fixed_tick();
        REQUIRE(robot1->_x == 19);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -201);REQUIRE(robot2->_y == 120);
        //GO
        game.fixed_tick();
        REQUIRE(robot1->_x == 18);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -202);REQUIRE(robot2->_y == 120);
        //GO
        game.fixed_tick();
        REQUIRE(robot1->_x == 17);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -203);REQUIRE(robot2->_y == 120);
        //GO
        game.fixed_tick();
        REQUIRE(robot1->_x == 16);REQUIRE(robot1->_y == 100);
        REQUIRE(robot2->_x == -204);REQUIRE(robot2->_y == 120);
    }
    
    SECTION("go block") {
        //TODO: add blocking that's not order dependent
        game.add_robot(10, 11);
        Robot* wait = game.get_robot(10, 11);
        game.add_robot(10, 10);
        Robot* block = game.get_robot(10, 10);
        game.set_program(parse_program("GO"));
        game.fixed_tick();
        REQUIRE(block->_y == 9);
        REQUIRE(wait->_y == 11);
    }
}
