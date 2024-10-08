#ifndef _PWM_H_
#define _PWM_H_

#include <stdint.h>
#include <stdbool.h>

void pwm_init(void);

void pwm_start(void);
void pwm_stop(const bool output_high);

void pwm_invert_out(void);
bool pwm_toggle_immediate_update(void);

bool pwm_set_freq(const uint32_t freq);
uint32_t pwm_get_freq(void);

bool pwm_set_duty(const uint32_t duty);
uint32_t pwm_get_duty(void);

#endif // _LED_H_
