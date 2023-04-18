#ifndef H_BOS_MEM
#define H_BOS_MEM

#include <stddef.h>

static inline void memcpy(uint8_t *Destination, uint8_t *Source, size_t N)
{
    while (N--) {
        *Source++ = *Destination++;
    }
}

#endif // H_BOS_MEM