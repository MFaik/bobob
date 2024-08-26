#pragma once

#include <string>
#include "text_editor.h"

class Game;

class ProgramWindow {
private:
    std::string _title;
    TextEditor _editor;
public:
    ProgramWindow(std::string title);

    bool draw(Game& game);
    void set_text(const std::string& text);
};
