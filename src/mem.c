#include <stdint.h>
#include <stddef.h>
#include "mem.h"
void memcpy(void *Destination_, void *Source_, size_t N)
{
    uint8_t *Destination = Destination_;
    uint8_t *Source = Source_;
    while (N--) {
        *Destination++ = *Source++;
    }
}
void memset(void *Destination_, uint8_t Val, size_t N)
{
    uint8_t *Destination = Destination_;
    while (N--) {
        *Destination++ = Val;
    }
}

// Each page is 4 kb
uint32_t Pages[1000000] = { 0 };


uint32_t* malloc(size_t Bytes)
{
    size_t PageCount = Bytes / 4096; 
    for (int i = 0;i < 1000000;i++)
    {
        if (!Pages[i])
        {
            uint8_t Found = 1;
            for (int j = i;j < i + PageCount;j++)
            {


                if (Pages[j])
                {
                    Found = 0;
                    break; 
                }
            }
            if (Found)
            {

                return (uint32_t*)(i * 4096 + 0x1000000);
            }
        }
    }
    return (uint32_t*)0;
}
void free(uint32_t* Buf, size_t Bytes)
{
    size_t PageCount = Bytes / 4096;

    uint32_t Page = ((uint32_t)Buf - 0x1000000) / 4096;
    
    for (uint32_t i = Page;i < Page + PageCount;i++)
    {
        Pages[i] = 0;
    }
}