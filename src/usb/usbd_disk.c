#include <stdint.h>

#include "usbd_msc.h"
#include "storage.h"

#define LUN_MAX 1
#define RET_OK  0

// USB Mass storage Standard Inquiry Data, 36 bytes
static int8_t _Inquirydata[] = {
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 5),
    0x00,
    0x00,
    0x00,
    'T', 'E', 'S', 'T', '_', '1', ' ', ' ', // Manufacturer: 8 bytes
    'F', '4', '_', 'D', 'i', 's', 'c', 'o', // Product: 16 Bytes
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '0', '.', '0', '1',                     // Version: 4 Bytes
};

static int8_t _Init(uint8_t lun);
static int8_t _GetCapacity(uint8_t lun, uint32_t* block_num, uint16_t* block_size);
static int8_t _IsReady(uint8_t lun);
static int8_t _IsWriteProtected(uint8_t lun);
static int8_t _Read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t _Write(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t _GetMaxLun(void);

// USB Device IO functions
USBD_StorageTypeDef usbd_disk =
{
    _Init,
    _GetCapacity,
    _IsReady,
    _IsWriteProtected,
    _Read,
    _Write,
    _GetMaxLun,
    _Inquirydata,
};

static int8_t _Init(uint8_t lun)
{
    (void)lun;
    return RET_OK;
}

static int8_t _GetCapacity(uint8_t lun, uint32_t* block_num, uint16_t* block_size)
{
    (void)lun;
    *block_num  = storage_get_block_num();
    *block_size = storage_get_block_size();
    return RET_OK;
}

static int8_t _IsReady(uint8_t lun)
{
    (void)lun;
    return RET_OK;
}

static int8_t _IsWriteProtected(uint8_t lun)
{
    (void)lun;
    return RET_OK;
}

static int8_t _Read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
    (void)lun;
    storage_read(buf, blk_addr, blk_len);
    return RET_OK;
}

static int8_t _Write(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
    (void)lun;
    storage_write(buf, blk_addr, blk_len);
    return RET_OK;
}

static int8_t _GetMaxLun(void)
{
  return (LUN_MAX - 1);
}
