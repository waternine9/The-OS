#include "bmp.h"

/* 
 * NOTE: IT CAN ONLY READ 32 BIT ARGB BMPS!
 */
void BMP_Read(uint8_t *DataPtr, bmp_bitmap_info *OutInfo, uint32_t *Destination)
{
    *OutInfo = (bmp_bitmap_info){ 0 };
    OutInfo->Width = *((uint32_t*)(DataPtr+18));
    OutInfo->Height = *((uint32_t*)(DataPtr+22));
    
    if (Destination) {
        uint32_t *TexLoc = (uint32_t*)(DataPtr+*((uint32_t*)(DataPtr+10)));
        
        for (int I = 0; I < OutInfo->Width*OutInfo->Height; I++) {
            Destination[I] = TexLoc[I];
        }
    }
}
