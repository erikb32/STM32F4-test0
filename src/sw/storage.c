#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "conf.h"
#include "storage.h"
#include "flash.h"
#include "led.h"
#include "uart.h"
#include "utils.h"
#include "ff_gen_drv.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

// convenience definitions
#define LED_FLASH_WRITE_ON      BSP_LED_On(LED_ORANGE)
#define LED_FLASH_WRITE_OFF     BSP_LED_Off(LED_ORANGE)
#define LED_FLASH_WRITE_ACTIVE  BSP_LED_Toggle(LED_ORANGE)

#define LED_WRITE_SUCCESS       BSP_LED_On(LED_GREEN)
#define LED_WRITE_SUCCESS_CLEAN BSP_LED_Off(LED_GREEN)
#define LED_WRITE_FAIL          BSP_LED_On(LED_RED)
#define LED_WRITE_FAIL_CLEAN    BSP_LED_Off(LED_RED)

#define SYNC_DELAY_MS   200u

extern const Diskio_drvTypeDef ff_disk;

static uint8_t _ram_blocks[STORAGE_BLOCKS][STORAGE_BLOCK_SIZE] = {0};

static FATFS _fat_fs;
static char _drive_path[4];
static volatile uint32_t _last_write_tick;

static inline const storage_item_head_t* _get_item_head(uint32_t adr)
{
    __IO uint8_t* mem_start = (uint8_t*)FLASH_STORE_AREA_ADR;
    return (const storage_item_head_t*)&mem_start[adr];
}

static inline bool _is_free(uint32_t adr)
{
    return (0xFFFFFFFF == _get_item_head(adr)->len);
}

static inline uint32_t _next_head_adr(uint32_t adr)
{
    return adr + sizeof(storage_item_head_t) + ALIGN_UP16(_get_item_head(adr)->len);
}

static void _erase_all(void)
{
    LED_WRITE_SUCCESS_CLEAN;
    LED_WRITE_FAIL_CLEAN;
    LED_FLASH_WRITE_ON;

    flash_erase_store();
    app_conf.c.lcd.item_nr = 0;
    conf_save();

    LED_FLASH_WRITE_OFF;
}

static bool _store_file(uint32_t adr, char* name, uint32_t len)
{
    uint32_t end = adr + sizeof(storage_item_head_t) + ALIGN_UP16(len);

    if (end < FLASH_AREA_MAX_LEN) {
        FIL fsrc;
        if (f_open(&fsrc, name, FA_READ) == FR_OK) {
            LED_FLASH_WRITE_ON;

            storage_item_head_t head;
            head.len = len;
            memcpy(head.name, name, sizeof(((storage_item_head_t){0}).name));
            flash_write_store_words(adr, (uint32_t*)&head, sizeof(storage_item_head_t) / 4u);
            adr += sizeof(storage_item_head_t);

            do {
                uint32_t read_buf[1024u];
                UINT bytes_read;

                if (f_read(&fsrc, read_buf, sizeof(read_buf), &bytes_read) == FR_OK) {
                    flash_write_store_words(adr, read_buf, (ALIGN_UP(bytes_read, 4u) / 4u));
                    adr += ALIGN_UP(bytes_read, 4u);
                    if (sizeof(read_buf) != bytes_read) {
                        LED_WRITE_SUCCESS;
                        break;  // End of file
                    }
                } else {
                    LED_WRITE_FAIL;
                    break;
                }

                LED_FLASH_WRITE_ACTIVE;
            } while (adr < end);
            f_close(&fsrc);

            uart_send_str("Saved: ");
            uart_send_str(name);
            uart_send_str("\r\n");

            LED_FLASH_WRITE_OFF;
            return true;
        }
    }
    return false;
}

static bool _sync_files(void)
{
    DIR dir;
    uint32_t adr = 0u;
    bool ret = true;

    // Look only root directory
    if (f_opendir(&dir, "") == FR_OK) {
        for (;;) {
            FILINFO finfo;
            FRESULT res = f_readdir(&dir, &finfo);
            if ((FR_OK != res) || (0 == finfo.fname[0])) {
                break; // End of directory
            }
            // Windows creates some crap, filter
            if ((0 == (finfo.fattrib & (AM_DIR | AM_SYS))) && (finfo.fsize > 0u)) {
                const storage_item_head_t* item = _get_item_head(adr);

                if (0xFFFFFFFF == item->len) {
                    if (_store_file(adr, finfo.fname, finfo.fsize) == true) {
                        adr = _next_head_adr(adr);
                    }
                } else {
                    if ((item->len == finfo.fsize) && (0 == strcmp(finfo.fname, item->name))) {
                        adr = _next_head_adr(adr);
                    } else {
                        ret = false;
                        break;
                    }
                }
            }
        }
        f_closedir(&dir);
    }

    if (_is_free(adr) == false) {
        // Too many items
        ret = false;
    }
    return ret;
}

static void _restore_files(void)
{
    uint32_t adr = 0u;
    while ((adr < FLASH_AREA_MAX_LEN) && (_is_free(adr) == false)) {
        const storage_item_head_t* item = _get_item_head(adr);

        FIL dest;
        if (FR_OK == f_open(&dest, item->name, FA_WRITE | FA_CREATE_ALWAYS)) {
            UINT bytes_written;
            f_write(&dest, (const void*)&item[1u], item->len, &bytes_written);
            f_close(&dest);
        }
        adr = _next_head_adr(adr);
    }
}

// -- Public Interface --
void storage_init(void)
{
    uint8_t work_buf[_MAX_SS];

    if (FATFS_LinkDriver(&ff_disk, _drive_path) == 0u) {
        if (f_mount(&_fat_fs, (TCHAR const*)_drive_path, 0u) == FR_OK) {
            if (f_mkfs((TCHAR const*)_drive_path, FM_FAT, 0u, work_buf, sizeof(work_buf)) == FR_OK) {
                _restore_files();
            }
        }
    }
    _last_write_tick = 0u;
}

bool storage_sync(void)
{
    if (0u != _last_write_tick) {
        uint32_t tick = HAL_GetTick();
        if ((tick - _last_write_tick) > SYNC_DELAY_MS) {
            f_mount(&_fat_fs, (TCHAR const*)_drive_path, 0u);
            _last_write_tick = 0u;

            if (_sync_files() == false) {
                _erase_all();
                (void)_sync_files();
                return true;
            }
        }
    }
    return false;
}

const storage_item_head_t* storage_get_item(uint8_t num)
{
    uint32_t adr = 0u;
    while ((adr < FLASH_AREA_MAX_LEN) && (_is_free(adr) == false)) {
        if (0u == num--) {
            return _get_item_head(adr); // Requested item
        }
        adr = _next_head_adr(adr);
    }
    return NULL; // No item
}

void storage_read(uint8_t* dest, uint32_t block, uint16_t count)
{
    memcpy(dest, &_ram_blocks[block][0u], (count * STORAGE_BLOCK_SIZE));
}

void storage_write(uint8_t* src, uint32_t block, uint16_t count)
{
    memcpy(&_ram_blocks[block][0u], src, (count * STORAGE_BLOCK_SIZE));
    _last_write_tick = HAL_GetTick();
}
