#ifndef H_BOS_MEM
#define H_BOS_MEM

#include <stddef.h>

static inline void memcpy(uint8_t *Destination, uint8_t *Source, size_t N)
{
    while (N--) {
        *Destination++ = *Source++;
    }
}
static inline void memset(uint8_t *Destination, uint8_t Val, size_t N)
{
    while (N--) {
        *Destination++ = Val;
    }
}

#endif // H_BOS_MEM