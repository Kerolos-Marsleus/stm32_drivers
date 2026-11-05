#include <cstddef>

constexpr size_t HEAP_SIZE = 2048U;

static char heap[HEAP_SIZE];
static char* ptr = heap;

void* operator new(size_t size){
    if((ptr-heap + size) > HEAP_SIZE ){
        return nullptr;
    }
    char* temp = ptr;
    ptr+=size;
    return temp;
}

void* operator new[](size_t size){
    return operator new(size);
}

void operator delete(void* p){
    // this program will never free its heap
    
    (void)p;
}

void operator delete[](void* p){
    // this program will never free its heap
    
    operator delete(p);
}