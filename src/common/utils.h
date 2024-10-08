#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#define ARRAYSIZE(array) (sizeof((array))/sizeof((array)[0]))

#define ALIGN_UP(val, al) (((val) + (al) - 1) & ~((al) - 1))
#define ALIGN_UP16(val) ALIGN_UP((val), 16)


static inline bool is_space(uint8_t ch)
{
	return ((uint8_t)' ' == ch) || ((uint8_t)(ch - '\t') < 5u);
}

uint32_t my_strtoul(uint8_t** buf_ptr);

// Simple blocking delays
void delay_ms(uint32_t val);
void delay_us(uint32_t val);

#endif // _UTILS_H_
