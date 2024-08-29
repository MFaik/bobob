#include "robot_window.h"

#include <raylib.h>
#include "imgui.h"
#include "rlImGui.h"

#include <string>

#include "item.h"
#include "assets.h"
extern Assets g_assets;


RobotWindow::RobotWindow(Robot* robot, ImVec2 start_pos) : 
    _robot(robot), _start_pos(start_pos) {
    static int counter = 0;
    counter++;
    id = counter;
}
//TODO: show current code line?
bool RobotWindow::draw() {
    if(_first_draw) {
        auto pivot = ImVec2(-0.1f, 0.5f);
        if(_start_pos.x > GetScreenWidth()-180) {
            pivot.x = 1.1f;
        }
        if(_start_pos.y < 180) {
            pivot.y = 0;
        } else if(_start_pos.y > GetScreenHeight()-180) {
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
            ImGui::Text("C: %d", _robot->_c);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("INPUT: %d", _robot->_input);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("COND: %d", _robot->_cond);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("PC: %d", _robot->_pc);
            ImGui::EndTable();
        }
        //ImGui::SameLine();
        if(ImGui::BeginTable("items", 4, ImGuiTableFlags_Borders)) {
            for(int i = 0;i < 16;i++) {
                ImGui::TableNextColumn();
                Item item = _robot->_inventory[i];
                if(item != Item::EMPTY) {
                    rlImGuiImage(&g_assets.get_texture(item));
                    ImGui::Text("%s", get_item_name(item));
                } else {
                    //TODO: fix this magic number
                    ImGui::Dummy(ImVec2(50, 50));
                    ImGui::Text("%s", "");
                }
            }
            ImGui::EndTable();
        }
        if(ret && ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive()) {
            ImGui::SetWindowFocus(nullptr);
        }
        ImGui::End();
    }
    return ret;
}
