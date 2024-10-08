#include "storage.h"
#include "ff_gen_drv.h"


static DSTATUS _ff_drv_init(BYTE lun);
static DSTATUS _ff_drv_status(BYTE lun);
static DRESULT _ff_drv_read(BYTE lun, BYTE* buf, DWORD block, UINT count);
#if _USE_WRITE == 1
    static DRESULT _ff_drv_write(BYTE lun, const BYTE* buf, DWORD block, UINT count);
#endif // _USE_WRITE
#if _USE_IOCTL == 1
    static DRESULT _ff_drv_ioctl(BYTE lun, BYTE cmd, void* buf);
#endif // _USE_IOCTL

const Diskio_drvTypeDef ff_disk =
{
    _ff_drv_init,
    _ff_drv_status,
    _ff_drv_read,
#if _USE_WRITE == 1
    _ff_drv_write,
#endif // _USE_WRITE
#if _USE_IOCTL == 1
    _ff_drv_ioctl,
#endif // _USE_IOCTL
};

static DSTATUS _ff_drv_init(BYTE lun)
{
    (void)lun;
    return RES_OK;
}

static DSTATUS _ff_drv_status(BYTE lun)
{
    (void)lun;
    return RES_OK;
}

static DRESULT _ff_drv_read(BYTE lun, BYTE* buf, DWORD block, UINT count)
{
    (void)lun;
    storage_read(buf, block, count);
    return RES_OK;
}

#if _USE_WRITE == 1
static DRESULT _ff_drv_write(BYTE lun, const BYTE* buf, DWORD block, UINT count)
{
    (void)lun;
    storage_write((uint8_t*)buf, block, count);
    return RES_OK;
}
#endif // _USE_WRITE

#if _USE_IOCTL == 1
static DRESULT _ff_drv_ioctl(BYTE lun, BYTE cmd, void* buf)
{
    (void)lun;
    DRESULT result = RES_OK;

    switch (cmd) {
        case CTRL_SYNC:
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*)buf = storage_get_block_num();
            break;

        case GET_SECTOR_SIZE:
            *(WORD*)buf = storage_get_block_size();
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD*)buf = 1u;
            break;

        default:
            result = RES_PARERR;
    }

    return result;
}
#endif // _USE_IOCTL
