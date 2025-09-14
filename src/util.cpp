#include "../lib/hw.h"

// memset is hardcoded to be used for initializing memory for larger structures
// if I were not to define it, the compiler would try to call a non-existing memset from stdlib
extern "C" void* memset(void* str, int c, size_t n)
{
    uint8 c8 = c;
    uint8* str8 = (uint8*)str;
    while (n > 0)
    {
        *str8 = c8;
        str8++;
        n--;
    }

    return str;
}