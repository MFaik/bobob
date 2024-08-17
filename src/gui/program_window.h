#pragma once

#include <string>
#include "text_editor.h"

class ProgramWindow {
private:
    std::string _title;
    TextEditor _editor;
public:
    ProgramWindow(std::string title);

    bool draw();
    void set_text(const std::string& text);
};
