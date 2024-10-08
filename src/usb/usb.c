#include "usb.h"
#include "usbd_desc.h"
#include "usbd_core.h"
#include "usbd_msc.h"

extern PCD_HandleTypeDef    my_hpcd;
extern USBD_StorageTypeDef  usbd_disk;

static USBD_HandleTypeDef USBD_Device;

void usb_init(void)
{
    USBD_Init(&USBD_Device, &MSC_Desc, 0);
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
    USBD_MSC_RegisterStorage(&USBD_Device, &usbd_disk);
    USBD_Start(&USBD_Device);
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&my_hpcd);
}
