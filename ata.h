#ifndef H_BOS_ATA
#define H_BOS_ATA
#include <stdint.h>
void ReadATASector(uint32_t amount, void* buffer, uint8_t primaryorsector, uint32_t lba);
#endif