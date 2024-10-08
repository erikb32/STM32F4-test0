#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdint.h>
#include <stdbool.h>

#define STORAGE_BLOCKS      256u
#define STORAGE_BLOCK_SIZE  512u

typedef struct
{
    uint32_t len;
    char name[12u];
} storage_item_head_t;

static inline uint32_t storage_get_block_num(void)
{
    return STORAGE_BLOCKS;
}

static inline uint16_t storage_get_block_size(void)
{
    return STORAGE_BLOCK_SIZE;
}

void storage_init(void);
bool storage_sync(void);

const storage_item_head_t* storage_get_item(uint8_t num);

void storage_read(uint8_t* dest, uint32_t block, uint16_t count);
void storage_write(uint8_t* src, uint32_t block, uint16_t count);

#endif // _STORAGE_H_
