
#include "../h/printing.hpp"

#include "../h/syscall_c.h"

void _printString(const char *string)
{
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
}

void _printInteger(uint64 integer, uint64 base, bool sign)
{
    if (base < 2 || base > 16){
        _printString("Error in printInteger: base not supported\n");
        return;
    }
    static char digits[] = "0123456789abcdef";
    char buf[16];
    int i, neg;
    uint64 x;

    neg = 0;
    if (sign && (long long)integer < 0)
    {
        neg = 1;
        x = -(long long)integer;
    } else
    {
        x = integer;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0) { putc(buf[i]); }
}