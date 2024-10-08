#include "flash.h"
#include "stm32f4xx_hal.h"

#define CONF_AREA   FLASH_SECTOR_10
#define FILE_AREA   FLASH_SECTOR_11

// 128k sector erase time max 2s
#define FLASH_TIMEOUT_MS    4000u

#define FLASH_ALL_ERROR_FLAGS   (FLASH_FLAG_EOP    | \
                                 FLASH_FLAG_OPERR  | \
                                 FLASH_FLAG_WRPERR | \
                                 FLASH_FLAG_PGAERR | \
                                 FLASH_FLAG_PGPERR | \
                                 FLASH_FLAG_PGSERR | \
                                 FLASH_FLAG_RDERR)

static void _erase(uint32_t sector)
{
    if (HAL_FLASH_Unlock() == HAL_OK) {
        __HAL_FLASH_CLEAR_FLAG(FLASH_ALL_ERROR_FLAGS);

        FLASH_Erase_Sector(sector, VOLTAGE_RANGE_3);
        FLASH_WaitForLastOperation(FLASH_TIMEOUT_MS);
        HAL_FLASH_Lock();
    }
}

static void _write_words(uint32_t* dest, const uint32_t* src, uint32_t len)
{
    if ((len > 0u) && (HAL_FLASH_Unlock() == HAL_OK)) {
        WRITE_REG(FLASH->CR, FLASH_PSIZE_WORD | FLASH_CR_PG);

        do {
            while (0u != __HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) { ; }
            *dest++ = *src++;
        } while (--len > 0u);
        while (0u != __HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) { ; }

        __HAL_FLASH_CLEAR_FLAG(FLASH_ALL_ERROR_FLAGS);
        WRITE_REG(FLASH->CR, 0u);
        HAL_FLASH_Lock();
    }
}

// -- Public Interface --
void flash_erase_store(void)
{
    _erase(FILE_AREA);
}

void flash_write_store_words(const uint32_t offset, const uint32_t* src, uint32_t len)
{
    uint32_t* dest = (uint32_t*)(FLASH_STORE_AREA_ADR + offset);
    _write_words(dest, src, len);
}

void flash_erase_conf(void)
{
    _erase(CONF_AREA);
}

void flash_write_conf_words(const uint32_t offset, const uint32_t* src, uint32_t len)
{
    uint32_t* dest = (uint32_t*)(FLASH_CONF_AREA_ADR + offset);
    _write_words(dest, src, len);
}