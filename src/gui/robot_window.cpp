#include <raylib.h>
#include <string>
#include "imgui.h"

#include "robot_window.h"

RobotWindow::RobotWindow(Robot* robot, ImVec2 start_pos) : 
    _robot(robot), _start_pos(start_pos) {
    static int counter = 0;
    counter++;
    id = counter;
}

bool RobotWindow::draw() {
    if(_first_draw) {
        auto pivot = ImVec2(-0.1f, 0.5f);
        if(_start_pos.x > GetScreenWidth()-100) {
            pivot.x = 1.1f;
        }
        if(_start_pos.y < 100) {
            pivot.y = 0;
        } else if(_start_pos.y > GetScreenHeight()-100) {
            pivot.y = 1.0f;
        }
        ImGui::SetNextWindowPos(_start_pos, ImGuiCond_Appearing, pivot);
        _first_draw = false;
    }

    ImGuiWindowFlags flag = 0;
    flag |= ImGuiWindowFlags_NoCollapse;
    flag |= ImGuiWindowFlags_NoSavedSettings;
    flag |= ImGuiWindowFlags_NoResize;
    bool ret = true;
    std::string name = "Robot##" + std::to_string(id);
    if(ImGui::Begin(name.c_str(), &ret, flag)) {
        if(ImGui::BeginTable("registers", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("A: %d", _robot->_a);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("B: %d", _robot->_b);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("INPUT: %d", _robot->_input);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("SEL: %d", _robot->_inventory_selector);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("COND: %d", _robot->_cond);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("PC: %d", _robot->_pc);
            ImGui::EndTable();
        }
        ImGui::End();
    }
    return ret;
}
