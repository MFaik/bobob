#pragma once

#include <cstring>
#include <cstdlib>
#include <list>

constexpr size_t ARENA_ALLOCATOR_MIN_SIZE = 32;

//TODO: what the fuck is going on here?
template <typename T>
class ArenaAllocator;

//ArenaPointer holds the ptr+1 to allow for 0 to be unset
template <typename T>
class ArenaPointer {
    private:
        friend class ArenaAllocator<T>;
        unsigned int i;
        ArenaPointer<T> operator+(int a) {
            return {i+a};
        }
    public:
        explicit ArenaPointer(int x) {
            i = x;
        }
        ArenaPointer() : i{0} {}
        bool operator==(ArenaPointer&& a) const& {
            return a.i == i;
        }
        bool operator==(const ArenaPointer& a) const& {
            return a.i == i;
        }
        bool empty() const {
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

        size_t _residual = 0;
        size_t _curr_max = 0;

        void double_size() {
            T* new_arena = (T*)calloc(_curr_max*2, sizeof(T));
            memcpy(new_arena, _arena, sizeof(T)*_curr_max);
            free(_arena);
            _arena = new_arena;
            _curr_max *= 2;
        }
    public:
        ArenaAllocator() {
            _arena = (T*)calloc(ARENA_ALLOCATOR_MIN_SIZE, sizeof(T));
            _curr_max = ARENA_ALLOCATOR_MIN_SIZE;
            _residual = 0;
        }
        ArenaPointer<T> allocate() {
            if(_empty.empty()) {
                if(_residual == _curr_max) {
                    double_size();
                }
                _residual++;
                return ArenaPointer<T>((unsigned int)_residual);
            } else {
                ArenaPointer<T> id = _empty.front();
                _empty.pop_front();
                return id;
            }
        }
        void resize(size_t new_size) {
            if(_curr_max >= new_size)
                return;
            T* new_arena = (T*)calloc(new_size, sizeof(T));
            memcpy(new_arena, _arena, sizeof(T)*_curr_max);
            free(_arena);
            _arena = new_arena;
            _curr_max = new_size;
        }
        void remove(ArenaPointer<T> ptr) {
            if(ptr.i) {
                _empty.push_back(ptr);
            }
        }
        void clear() {
            memset(_arena, 0, _curr_max * sizeof(T));
            _empty.clear();
            _residual = 0;
        }
        T* get(ArenaPointer<T> ptr) {
            if(ptr.i)
                return _arena+ptr.i-1;
            else
                return nullptr;
        }
        size_t size() {
            return _residual - _empty.size();
        }
        size_t capacity() {
            return _curr_max;
        }
        ~ArenaAllocator() {
            free(_arena);
        }

        ArenaAllocator& operator=(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(ArenaAllocator&&) = delete;
};


