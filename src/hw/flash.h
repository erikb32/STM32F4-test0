#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

#define FLASH_AREA_MAX_LEN      ((128u * 1024u) - 32u)
#define FLASH_CONF_AREA_ADR     0x080C0000
#define FLASH_STORE_AREA_ADR    0x080E0000

void flash_erase_store(void);
void flash_write_store_words(const uint32_t offset, const uint32_t* src, uint32_t len);

void flash_erase_conf(void);
void flash_write_conf_words(const uint32_t offset, const uint32_t* src, uint32_t len);

#endif // _FLASH_H_
