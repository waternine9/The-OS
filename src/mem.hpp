#ifndef H_MEM
#define H_MEM

#include <stdint.h>
#include <stddef.h>

void kmemcpy(void *Destination_, const void *Source_, size_t N);
void kmemset(void *Destination_, uint8_t Val, size_t N);
void *kmalloc(size_t Bytes);
void kfree(void *Buf);

#endif // H_MEM
