#include <stdint.h>

#include "command.h"
#include "conf.h"
#include "jpeg.h"
#include "lcd.h"
#include "led.h"
#include "pwm.h"
#include "storage.h"
#include "system.h"
#include "touch.h"
#include "uart.h"
#include "usb.h"

static const char APP_BANNER[] =
"********************************\r\n"
"  STM32F412_TEST v0.0.1         \r\n"
"  Built: "__DATE__" "__TIME__"  \r\n"
"********************************\r\n";

static void _print_info(void)
{
    uart_send((const uint8_t*)&APP_BANNER[0], sizeof(APP_BANNER));

    // Show current settings
    char str_buf[64u];
    snprintf(str_buf, sizeof(str_buf), "PWM freq: %luHz, Duty: %u%% %s\r\n",
        app_conf.c.pwm.freq, app_conf.c.pwm.duty, (0u != app_conf.c.pwm.inv) ? "INV" : "");
    uart_send_str(str_buf);

    snprintf(str_buf, sizeof(str_buf), "Active item nr: %u\r\n", app_conf.c.lcd.item_nr);
    uart_send_str(str_buf);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    conf_init();
    led_init();
    uart_init();
    pwm_init();
    storage_init();
    usb_init();
    lcd_init();
    touch_init();
    jpeg_init();

    // -- init done --
    pwm_start();

    _print_info();
    touch_info();
    command_prompt();

    bool is_image_displayed = command_exec(DISPLAY);
    for (;;) {
        led_blinky();

        if ((true == storage_sync()) || (false == is_image_displayed)) {
            is_image_displayed = command_exec(DISPLAY);
        }
        if (true == is_image_displayed) {
            touch_poll();
        }

        command_check();
    }

    return 0;
}
