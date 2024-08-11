#pragma once

#include <string>
#include "text_editor.h"

class ProgramWindow {
private:
    std::string _title;
    TextEditor _editor;
public:
    ProgramWindow(std::string title);

    void draw();
};
