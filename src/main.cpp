#include <imgui.h>
#include <raylib.h>
#include "rlImGui.h"
 
#include <fstream>

#include "item.h"
#include "game.h"
#include "util.h"

#include "game_ui.h"
extern GameUI g_game_ui;

#include "assets.h"
extern Assets g_assets;

int main() {
    // auto prog = parse_program("SEL 2\nMOV A 0\nloop:\nUSE\ngo_retry:\nGO\nCMP INPUT BLOCKED\nJEQ go_retry\nADD A 1\nCMP A 10\n JNE loop\nTURN RIGHT");
    // auto prog = parse_program("ADD B RIGHT\nloop1:\nMOV A 0\nloop2:\nGO\nADD A 1\nCMP A 50\nJNE loop2\nTURN B\nCMP B RIGHT\nMOV B 0\nJNE SKIP\nMOV B 2\nSKIP:");
    // auto prog = parse_program("NOP");
    // g_game.setup_program(prog);
    //
    // for(auto e : prog._errors) {
    //     std::cout << e.txt << std::endl;
    // }
    //
    // if(prog._errors.size()) {
    //     return 1;
    // }
    //
    // g_game.get_tile_ref(0, 0).type = Item::STONE;
    // g_game.get_tile_ref(0, 4).type = Item::STONE;
    //
    std::array<Item, 16> inv{};
    inv[0] = Item::APPLE;
    inv[1] = Item::APPLE;
    inv[2] = Item::APPLE;
    inv[3] = Item::APPLE;
    inv[4] = Item::STONE;
    inv[5] = Item::CHARCOAL;
    inv[6] = Item::STONE;
    inv[15] = Item::BOX;

    Game game;
    game.add_robot(-1, -1, inv);
    // 
    // for(int y = 10;y < 1000;y++) {
    //     for(int x = 10;x < 1000;x++) {
    //         g_game.add_robot(x, y, inv);
    //     }
    // }
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "bobob");
    ToggleFullscreen();
    SetTargetFPS(60);
    SetExitKey(0);
    rlImGuiSetup(true);
    int fixed_cnt = 0;

    g_assets.load_textures();
    while(!g_assets.is_textures_loaded());
    
    Camera2D camera{};
    camera.target = {-GetScreenWidth()/2.0f, -GetScreenHeight()/2.0f};
    camera.zoom = 1;
    game.set_camera(camera);

    int speed = 1;
    std::array<int, 4> speed_tick_cnt = { 20, 10, 5, 2 };
    std::array<const char*, 4> speed_name = {"0.5x", "1x", "2x", "4x"};
    while(!WindowShouldClose()) {
        BeginDrawing();
        if(fixed_cnt > speed_tick_cnt[speed]) {
            game.fixed_tick();
            fixed_cnt = 0;
        } else {
            fixed_cnt++;
        }
        
        game.draw();
        
        rlImGuiBegin();

        //ImGui::ShowDemoWindow();

        if(ImGui::Button("Save")) {
            std::string save = game.save_game();
            std::ofstream file("test.bobob", std::ios::out|std::ios::binary|std::ios::trunc);
            file << save;
        }
        ImGui::SameLine();
        if(ImGui::Button("Load")) {
            std::string save = get_file_contents("test.bobob");
            game.load_game(save);
            g_game_ui.set_program_text(game.get_program());
        }
        if(ImGui::Button(game.is_paused() ? "Play" : "Pause")) {
            game.toggle_pause();
        }
        if(game.is_paused()) {
            ImGui::SameLine();
            if(ImGui::Button("Step")) {
                game.fixed_tick(true);
            }
        } else {
            ImGui::SameLine();
            if(ImGui::Button(speed_name[speed])) {
                speed = (speed+1)%speed_tick_cnt.size();
            }
        }

        g_game_ui.draw(game);

        ImGuiHoveredFlags mouse_flag = 0;
        mouse_flag |= ImGuiHoveredFlags_AnyWindow;
        mouse_flag |= ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
        bool mouse_free = !ImGui::IsWindowHovered(mouse_flag);
        bool keyboard_free = !ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
        
        rlImGuiEnd();
        
        game.tick(GetFrameTime(), mouse_free, keyboard_free);

        DrawFPS(0, 0);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
}
