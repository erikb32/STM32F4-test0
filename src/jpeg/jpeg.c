#include "jpeg.h"
#include "lcd.h"

#define JPEG_STREAM_SIGNATURE 0xE0FFD8FFu

typedef struct RGB
{
    uint8_t B;
    uint8_t G;
    uint8_t R;
} RGB_typedef;

struct jpeg_decompress_struct cinfo;

static uint8_t _row_buf[2048];

void jpeg_init(void)
{
    // Note: needs better error handling
    static struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
}

bool jpeg_show(uint8_t* data, uint32_t data_len)
{
    if (JPEG_STREAM_SIGNATURE != *((uint32_t*)data)) {
        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, data_len);
    bool is_image = (jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK);

    if (true == is_image) {
        cinfo.dct_method = JDCT_FLOAT;
        jpeg_start_decompress(&cinfo);
        lcd_clear();

        JSAMPROW buffer[2] = {0};
        buffer[0] = _row_buf;
        uint16_t line = 0u;
        while ((cinfo.output_scanline < cinfo.output_height) && (line < LCD_HEIGHT)) {
            (void)jpeg_read_scanlines(&cinfo, buffer, 1u);

            RGB_typedef* m = (RGB_typedef*)buffer[0];
            for (uint16_t i = 0u; i < LCD_WIDTH; i++) {
                uint16_t pixel = (uint16_t) (
                    ((m[i].R & 0x00F8) >> 3) |
                    ((m[i].G & 0x00FC) << 3) |
                    ((m[i].B & 0x00F8) << 8)
                );
                BSP_LCD_DrawPixel(i, line, pixel);
            }
            line++;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return is_image;
}

bool jpeg_info(uint8_t* data, uint32_t data_len)
{
    if (JPEG_STREAM_SIGNATURE != *((uint32_t*)data)) {
        return false;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, data_len);

    // Note: some invalid streams cause stuck
    bool is_image = (jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return is_image;
}
