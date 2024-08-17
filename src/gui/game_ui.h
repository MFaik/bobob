#include <unordered_map>

#include "program_window.h"
#include "robot.h"
#include "robot_window.h"

class GameUI {
    ProgramWindow _program_window;
    std::unordered_map<Robot*, RobotWindow> _robot_windows;
public:
    GameUI();

    void draw();

    void add_robot_window(Robot* robot);
    void remove_robot_window(Robot* robot);
    void reset_robot_windows();

    void set_program_text(const std::string& program);
};
