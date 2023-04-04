#ifndef H_BOS_ATA
#define H_BOS_ATA
#include <stdint.h>
void ReadATASector(void* buff, uint32_t lba);
void WriteATASector(void* buff, uint32_t lba);
void ATASetPIO();
#endif