#pragma once
class Map;

class TileUpdate {
public:
    //these are defined in priority order
    enum Type {
        BURN,
        APPLE,
        FLOW,
    };
//TODO: make these private
// private:
    int x;
    int y;
    Type type;
    TileUpdate(int x, int y, Type type):x(x), y(y), type(type) {}
    void update(Map& map);
private:
    void push_update_around(Map& map, Type type);
    void update_apple(Map& map);
    void update_burn(Map& map);
    void update_flow(Map& map);
};
