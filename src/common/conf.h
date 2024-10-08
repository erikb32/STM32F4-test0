#ifndef _CONF_H_
#define _CONF_H_

#include <stdint.h>
#include <stdbool.h>

typedef union
{
    struct {
        uint16_t version;
        struct {
            uint32_t freq;
            uint16_t duty;
            uint8_t inv;
        } pwm;
        struct {
            uint8_t item_nr;
        } lcd;
    } c;
    uint32_t raw_data[4u];
} conf_v0_t;

extern conf_v0_t app_conf;

void conf_init(void);
bool conf_is_valid(void);

void conf_save(void);

#endif // _CONF_H_
