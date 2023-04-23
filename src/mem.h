#ifndef H_TOS_MEM
#define H_TOS_MEM

#include <stddef.h>

void memcpy(void *Destination_, void *Source_, size_t N);
void memset(void *Destination_, uint8_t Val, size_t N);
uint32_t* malloc(size_t Bytes);
void free(uint32_t* Buf, size_t Bytes);
#endif // H_TOS_MEM