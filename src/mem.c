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
uint8_t Pages[100000] = { 0 };

uint8_t IsInit = 0;

uint32_t* malloc(size_t Bytes)
{
    if (!IsInit)
    {
        memset(Pages, 0, 100000);
        IsInit = 1;
    }
    size_t PageCount = Bytes / 4096 + 1; 
    for (int i = 0;i < 100000;i++)
    {
        if (!Pages[i])
        {
            uint8_t Found = 1;
            for (int j = i;j < i + PageCount;j++)
            {


                if (Pages[j])
                {
                    i = j;
                    Found = 0;
                    break; 
                }
            }
            if (Found)
            {

                for (int j = i;j < i + PageCount;j++)
                {
                    Pages[j] = 1;
                }   
                return (uint32_t*)(i * 4096 + 0x2000000);
            }
        }
    }
    return (uint32_t*)0;
}
void free(uint32_t* Buf, size_t Bytes)
{
    size_t PageCount = Bytes / 4096 + 1;

    uint32_t Page = ((uint32_t)Buf - 0x2000000) / 4096;
    
    for (uint32_t i = Page;i < Page + PageCount;i++)
    {
        Pages[i] = 0;
    }
}