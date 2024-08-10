#pragma once
#include <string>

#include "gui_widget.h"

class GuiWindow : public GuiWidget {
public:
    void tick() override;
    bool draw() override;
    
    GuiWindow(Vector2 pos, Vector2 window_size, std::string name);
    Rectangle get_window_bounds();
    void change_title(std::string title);
private:
    bool _is_moving;
    std::string _title;
};
