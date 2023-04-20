#ifndef H_TOS_MEM
#define H_TOS_MEM

#include <stddef.h>

static inline void memcpy(void *Destination_, void *Source_, size_t N)
{
    uint8_t *Destination = Destination_;
    uint8_t *Source = Source_;
    while (N--) {
        *Destination++ = *Source++;
    }
}
static inline void memset(void *Destination_, uint8_t Val, size_t N)
{
    uint8_t *Destination = Destination_;
    while (N--) {
        *Destination++ = Val;
    }
}

#endif // H_TOS_MEM