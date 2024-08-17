#include <tuple>
#include <utility>
#include "imgui.h"

#include "game_ui.h"

GameUI g_game_ui;

GameUI::GameUI() : _program_window("program.bobob"){}

void GameUI::draw() {
    for(auto rw = _robot_windows.begin();rw != _robot_windows.end();) {
        auto curr = rw;
        rw++;
        if(!curr->second.draw()) {
            _robot_windows.erase(curr);
        }
    }
    _program_window.draw();
}

void GameUI::add_robot_window(Robot* robot) {
    if(_robot_windows.find(robot) == _robot_windows.end()) {
        _robot_windows.emplace(std::piecewise_construct,
                               std::forward_as_tuple(robot), 
                               std::forward_as_tuple(robot, ImGui::GetMousePos()));
    }
}

void GameUI::remove_robot_window(Robot* robot) {
    _robot_windows.erase(robot);
}

void GameUI::reset_robot_windows() {
    _robot_windows.clear();
}

void GameUI::set_program_text(const std::string& program) {
    _program_window.set_text(program);
}
