#ifndef H_BOS_ATA
#define H_BOS_ATA
#include <stdint.h>
void ReadATASector(void* buffer, uint8_t primaryorsector, uint32_t lba);
void ATASetPIO();
#endif