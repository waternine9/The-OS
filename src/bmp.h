#ifndef H_TOS_BMP
#define H_TOS_BMP

#include <stdint.h>

typedef struct {
    uint32_t Width;
    uint32_t Height;
} bmp_bitmap_info;

/*
 * BMP Reader, returns ARGB data in `Destination`, NULL if you just want to query the info.
 */
void BMP_Read(uint8_t *DataPtr, bmp_bitmap_info *OutInfo, uint32_t *Destination);

#endif // H_TOS_BMP
