#ifndef H_BOS_MEM
#define H_BOS_MEM

#include <stddef.h>

static inline void memcpy(uint8_t *Destination, uint8_t *Source, size_t N)
{
    for (int I = 0; I < N; I++) {
        Destination[I] = Source[I];
    }
}

#endif // H_BOS_MEM