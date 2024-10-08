#include "utils.h"

uint32_t my_strtoul(uint8_t** buf_ptr)
{
    const uint8_t BASE = 10u;

    uint8_t* buf = *buf_ptr;
    uint32_t result = 0u;
    uint8_t ch;

	do {
		ch = *buf++;
	} while (is_space(ch));

	for (; ; ch = *buf++) {
        ch -= (uint8_t)'0';
		if (ch >= BASE) {
			break;
        }
        if (result > (result * BASE)) {
            break; // overflow
        }
        result *= BASE;
        result += ch;
    }

    *buf_ptr = buf - 1u;
    return result;
}

// unit close (less) to miillisecond
void delay_ms(uint32_t val)
{
    (void)val;
    __asm(".syntax unified    \n"
          "lsls	r0, r0, #15   \n"
          "__my_delay_ms:     \n"
          "subs r0, r0, #1    \n"
          "bhi  __my_delay_ms \n"
          ".syntax divided");
}

// unit close (less) to microsecond
void delay_us(uint32_t val)
{
    (void)val;
    __asm(".syntax unified    \n"
          "lsls	r0, r0, #5    \n"
          "__my_delay_us:     \n"
          "subs r0, r0, #1    \n"
          "bhi  __my_delay_us \n"
          ".syntax divided");
}
