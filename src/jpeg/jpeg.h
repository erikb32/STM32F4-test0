#ifndef _JPEG_H_
#define _JPEG_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "jpeglib.h"

extern struct jpeg_decompress_struct cinfo;

void jpeg_init(void);
bool jpeg_show(uint8_t* data, uint32_t data_len);
bool jpeg_info(uint8_t* data, uint32_t data_len);

#endif // _JPEG_H_
