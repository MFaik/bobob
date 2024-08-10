#include <array>
#include <list>

#include "gui_widget.h"

const size_t LAYER_COUNT = 3;

class GuiEngine {
private:
    std::array<std::list<GuiWidget*>, LAYER_COUNT> widgets;
public:
    void add_widget(GuiWidget *widget, size_t layer);
    bool draw();
};
