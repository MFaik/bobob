#include "raygui.h"
#include <raylib.h>

#include "gui_engine.h"

void GuiEngine::add_widget(GuiWidget *widget, size_t layer) {
    widgets[layer].push_back(widget);
}

bool GuiEngine::draw() {
    GuiLock();
    std::list<GuiWidget*>::iterator top = widgets[0].end();
    size_t top_layer = -1;
    for(size_t layer = 0;layer < widgets.size();layer++) {
        auto& list = widgets[layer];
        for(auto it = list.begin();it != list.end();it++) {
            (*it)->tick();

            bool collides = CheckCollisionPointRec(GetMousePosition(), (*it)->bounds);
            if(collides) {
                top = it;
                top_layer = layer;
            }
        }
    }
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && top != widgets[0].end() &&
        widgets[top_layer].back() != *top) {
        auto widget = *top;
        widgets[top_layer].erase(top);
        widgets[top_layer].push_back(widget);
        top = widgets[top_layer].end();
        top--;
    }
    for(auto& list : widgets) {
        for(auto it = list.begin();it != list.end();) {
            auto curr = it;
            it++;

            if(top == curr) {
                GuiUnlock();
            }
            
            bool erase = (*curr)->draw();

            if(top == curr) {
                if(erase) {
                    delete *curr;
                    list.erase(curr);
                }
                GuiLock();
            }
        }
    }
    GuiUnlock();
    return top_layer+1;
}
