#ifndef H_TOS_MEM
#define H_TOS_MEM

#include <stddef.h>
#include <stdint.h>

void memcpy(void *Destination_, const void *Source_, size_t N);
void memset(void *Destination_, uint8_t Val, size_t N);
void *malloc(size_t Bytes);
void free(void *Buf, size_t Bytes);
void *memmove(void *dest, const void *src, size_t n);
#endif // H_TOS_MEM