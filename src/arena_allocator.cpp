#include <cstring>

#include "arena_allocator.h"
#include "robot.h"

template class ArenaAllocator<Robot>;

template <typename T>
ArenaAllocator<T>::ArenaAllocator() {
    _arena = (T*)calloc(32, sizeof(T));
    _curr_max = 32;
    _residual = 0;
}

template <typename T>
ArenaPointer<T> ArenaAllocator<T>::allocate() {
    if(_empty.empty()) {
        if(_residual == _curr_max) {
            double_size();
        }
        _residual++;
        return {_residual};
    } else {
        ArenaPointer<T> id = _empty.front();
        _empty.pop_front();
        return id;
    }
}

template <typename T>
void ArenaAllocator<T>::remove(ArenaPointer<T> ptr) {
    if(ptr.i) {
        _empty.push_back(ptr);
    }
}

template <typename T>
T* ArenaAllocator<T>::get(ArenaPointer<T> ptr) {
    if(ptr.i)
        return _arena+ptr.i-1;
    else
        return nullptr;
}

template <typename T>
void ArenaAllocator<T>::double_size() {
    T* new_arena = (T*)calloc(_curr_max*2, sizeof(T));
    memcpy(new_arena, _arena, sizeof(T)*_curr_max);
    free(_arena);
    _arena = new_arena;
    _curr_max *= 2;
}

template <typename T>
ArenaAllocator<T>::~ArenaAllocator() {
    free(_arena);
}
