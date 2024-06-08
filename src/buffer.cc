#include <stdlib.h>
#include <cstring>
#include "buffer.h"

Buffer::Buffer()
{
    pos = 0;
    sz = 512;
    buf = malloc(sz * 8);
}

void Buffer::set_at(size_t ix, double val)
{
    double *dp = (double*)buf;
    dp += ix;
    *dp = val;
}

void Buffer::push(double val)
{
    if (pos == sz)
    {
        void *newBuf = malloc(sz * 2 * 8);
        memset(newBuf, 0, sz * 2 * 8);
        memcpy(newBuf, buf, sz * 8);
        free(buf);
        buf = newBuf;
        sz *= 2;
    }
    double *dp = (double*)buf;
    dp += pos;
    *dp = val;
    ++pos;
}