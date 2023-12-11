#include <cstdint>
#include <cstdlib>

#include "test_memory.hpp"

size_t HEAPSIZE = 0;

size_t heapSize()
{
    return HEAPSIZE;
}

void * operator new(std::size_t n)
{
    char* ptr = (char*)malloc(n + sizeof(size_t));
    *((std::size_t*)ptr) = n;
    HEAPSIZE += n;
    return (void*)(ptr + sizeof(std::size_t));
}
void operator delete(void * p)
{
    char* ptr = ((char*)p) - sizeof(std::size_t);

    HEAPSIZE -= *(std::size_t*)(ptr);

    free(ptr);
}