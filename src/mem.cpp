#include <stdint.h>
#include <stddef.h>
#include "mem.hpp"
#include "mutex.hpp"

void kmemcpy(void *Destination_, const void *Source_, size_t N)
{
    uint8_t *Destination = (uint8_t*)Destination_;
    const uint8_t *Source = (uint8_t*)Source_;
    while (N--) {
        *Destination++ = *Source++;
    }
}
void kmemset(void *Destination_, uint8_t Val, size_t N)
{
    uint8_t *Destination = (uint8_t*)Destination_;
    while (N--) {
        *Destination++ = Val;
    }
}

// Each page is 512 bytes
uint8_t Pages[1000000] = { 0 };

uint8_t IsInit = 0;

mutex MemMutex = { 0 };

void *kmalloc(size_t Bytes)
{
    MutexLock(&MemMutex);
    if (!IsInit)
    {
        kmemset(Pages, 0, 1000000);
        IsInit = 1;
    }
    size_t PageCount = Bytes / 4096 + 1;
    for (int i = 0;i < 1000000;i++)
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
                *(uint32_t*)(i * 512 + 0x2000000 - 4) = PageCount;
                MutexRelease(&MemMutex);
                return (uint32_t*)(i * 512 + 0x2000000);
            }
        }
    }
    MutexRelease(&MemMutex);
    return (uint32_t*)0;
}
void kfree(void *Buf)
{
    uint32_t PageCount = *(uint32_t*)(Buf - 4);

    uint32_t Page = ((uint32_t)Buf - 0x2000000) / 512;
    
    for (uint32_t i = Page;i < Page + PageCount;i++)
    {
        Pages[i] = 0;
    }
}
