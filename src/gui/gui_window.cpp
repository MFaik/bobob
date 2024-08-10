#include "gui_window.h"

#include <raylib.h>
#include "raygui.h"

const int left_padding = 0;
const int right_padding = 0;
const int top_padding = 24;
const int bottom_padding = 0;

GuiWindow::GuiWindow(Vector2 pos, Vector2 window_size, std::string _title) : 
    _title(_title) {
    bounds = Rectangle{pos.x, pos.y, 
        (window_size.x+left_padding+right_padding), 
        (window_size.y+top_padding+bottom_padding)
    };
}

Rectangle GuiWindow::get_window_bounds() {
    return Rectangle{bounds.x+left_padding, bounds.y+top_padding, 
                     bounds.width-left_padding-right_padding,
                     bounds.height-top_padding-bottom_padding};
}

void GuiWindow::change_title(std::string title) {
    _title = title;
}

bool GuiWindow::draw() {
    if(!_is_moving && !GuiIsLocked() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
            CheckCollisionPointRec(
                GetMousePosition(), 
                Rectangle{bounds.x, bounds.y, bounds.width, top_padding}
                )
      ){
        _is_moving = true;
    }

    int ret = GuiWindowBox(bounds, _title.c_str());

    return ret;
}

void GuiWindow::tick() {
    if(_is_moving) {
        if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            _is_moving = false;
        } else {
            bounds.x += GetMouseDelta().x;
            bounds.y += GetMouseDelta().y;
        }
    }
}
