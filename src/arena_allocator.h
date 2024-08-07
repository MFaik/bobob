#pragma once

#include <cstdlib>
#include <list>
#include <type_traits>

//TODO: what the fuck is going on here?
template <typename T>
class ArenaAllocator;

//ArenaPointer holds the ptr+1 to allow for 0 to be unset
template <typename T>
class ArenaPointer {
private:
    friend class ArenaAllocator<T>;
    int i;
    ArenaPointer<T> operator+(int a) {
        return {i+a};
    }
    ArenaPointer(int x) {
        i = x;
    }
public:
    ArenaPointer() : i{0} {}
    bool operator==(ArenaPointer&& a) {
        return a.i == i;
    }
    bool operator==(const ArenaPointer& a) {
        return a.i == i;
    }
    bool operator!() {
        return !i;
    }
};

//this only works for trivial types that can be 
//malloc, memset and memcpy
template<typename T>
class ArenaAllocator {
private:
    T* _arena;

    std::list<ArenaPointer<T>> _empty;

    int _residual = 0;
    int _curr_max = 0;

    void double_size();
public:
    ArenaAllocator();
    ArenaPointer<T> allocate();
    void remove(ArenaPointer<T> i);
    T* get(ArenaPointer<T>);
    ~ArenaAllocator();

    ArenaAllocator& operator=(const ArenaAllocator&) = delete;
    ArenaAllocator& operator=(ArenaAllocator&&) = delete;
};


