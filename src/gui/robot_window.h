#include "imgui.h"

#include "robot.h"

class RobotWindow {
    Robot* _robot;
    ImVec2 _start_pos;
    bool _first_draw = true;
public:
    RobotWindow(Robot* robot, ImVec2 start_pos);
    bool draw();
    int id;
};
