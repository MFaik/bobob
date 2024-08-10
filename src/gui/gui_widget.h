#pragma once
#include <raylib.h>

class GuiWidget {
public:
    virtual void tick() = 0;
    virtual bool draw() = 0;
    Rectangle bounds;
    
    virtual ~GuiWidget() {};
};
