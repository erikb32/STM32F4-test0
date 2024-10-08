#include <stdint.h>
#include <stdio.h>

#include "command.h"
#include "conf.h"
#include "jpeg.h"
#include "flash.h"
#include "lcd.h"
#include "pwm.h"
#include "storage.h"
#include "uart.h"
#include "utils.h"
#include "touch.h"
#include "stm32f4xx_hal.h"

#define MAX_STR_LEN 64u

typedef void (*handler_func_t)(uint8_t* buf);

typedef struct
{
    union {
        char name[4u];
        uint32_t id;
    } d;
    handler_func_t handler;
    const char* description;
} cmd_t;

static void _freq_cmd(uint8_t* buf);
static void _duty_cmd(uint8_t* buf);
static void _invert_out(uint8_t* buf);
static void _set_update(uint8_t* buf);
static void _list_items(uint8_t* buf);
static void _erase_all(uint8_t* buf);
static void _do_reset(uint8_t* buf);
static void _print_cmds(uint8_t* buf);
static void _help_cmd(uint8_t* buf);
static void _show_image(uint8_t* buf);
static void _show_next(uint8_t* buf);
static void _show_prev(uint8_t* buf);

static const cmd_t commands[] =
{
    {.d.name = "freq", .handler = _freq_cmd,   .description = "Get/set PWM frequency, range 1..999999"},
    {.d.name = "duty", .handler = _duty_cmd,   .description = "Get/set PWM duty cycle, range 0..100"},
    {.d.name = "inv",  .handler = _invert_out, .description = "Invert PWM out"},
    {.d.name = "set",  .handler = _set_update, .description = "Toggle between immediate PWM conf update"},
    {.d.name = "list", .handler = _list_items, .description = "List stored items"},
    {.d.name = "show", .handler = _show_image, .description = "Display stored image [Item nr]"},
    {.d.name = "next", .handler = _show_next,  .description = "Display next stored image"},
    {.d.name = "prev", .handler = _show_prev,  .description = "Display previous stored image"},
    {.d.name = "erse", .handler = _erase_all,  .description = "Erase conf and stored items"},
    {.d.name = "help", .handler = _help_cmd,   .description = "Print commands and description"},
    {.d.name = "cmd",  .handler = _print_cmds, .description = "List all commands"},
    {.d.name = "rst",  .handler = _do_reset,   .description = "System reset"},
};

static void _do_reset(uint8_t* buf)
{
    (void)buf;

    uart_send_str("RESET..\r\n\n");
    delay_ms(20u);
    HAL_NVIC_SystemReset();
}

static void _show_next(uint8_t* buf)
{
    (void)buf;

    uint8_t num = app_conf.c.lcd.item_nr;
    uint8_t next = num + 1u;

    while (next != num) {
        const storage_item_head_t* item = storage_get_item(next);
        if (NULL != item) {
            if (true == jpeg_show((uint8_t*)(&item[1u]), item->len)) {
                uart_send_str("Next: ");
                uart_send_str(item->name);
                uart_send_str("\r\n");

                app_conf.c.lcd.item_nr = next;
                conf_save();
                return;
            }
            next++;
        } else {
            next = 0;
        }
    }
    uart_send_str("No next image\r\n");
}

static void _show_prev(uint8_t* buf)
{
    (void)buf;

    uint8_t num = app_conf.c.lcd.item_nr;
    uint8_t next = num - 1u;

    while (next != num) {
        const storage_item_head_t* item = storage_get_item(next);
        if (NULL != item) {
            if (true == jpeg_show((uint8_t*)(&item[1u]), item->len)) {
                uart_send_str("Next: ");
                uart_send_str(item->name);
                uart_send_str("\r\n");

                app_conf.c.lcd.item_nr = next;
                conf_save();
                return;
            }
        }
        next--;
    }
    uart_send_str("No previous image\r\n");
}

// Show and set active image
static void _show_image(uint8_t* buf)
{
    uint32_t num = 0u;
    if (0 != *buf) {
        num = my_strtoul(&buf);
        if ((0 != *buf) || (num > 255u)){
            uart_send_str("Invalid value\r\n");
            return;
        }
    }

    const storage_item_head_t* item = storage_get_item(num);
    if (NULL != item) {
        uart_send_str("Show: ");
        uart_send_str(item->name);
        if (true == jpeg_show((uint8_t*)(&item[1u]), item->len)) {
            app_conf.c.lcd.item_nr = num;
            conf_save();
        } else {
            uart_send_str(" failed");
        }
        uart_send_str("\r\n");
    } else {
        uart_send_str("No item\r\n");
    }
}

// Display only if exist
static bool _display_image(void)
{
    bool is_image_ok = false;

    uint8_t num = app_conf.c.lcd.item_nr; // Try configured item first
    const storage_item_head_t* item = storage_get_item(num);
    if (NULL != item) {
        is_image_ok = jpeg_show((uint8_t*)(&item[1u]), item->len);
    }

    num = 0u; // also try all from start
    while (false == is_image_ok) {
        item = storage_get_item(num++);
        if (NULL == item) {
            break;
        }
        is_image_ok = jpeg_show((uint8_t*)(&item[1u]), item->len);
    }

    if (true == is_image_ok) {
        lcd_on();
    } else {
        lcd_off();
    }
    return is_image_ok;
}

static void _erase_all(uint8_t* buf)
{
    (void)buf;

    uart_send_str("Erase flash.. ");
    delay_ms(10u);
    flash_erase_store();
    flash_erase_conf();
    uart_send_str("done \r\n");
}

static void _list_items(uint8_t* buf)
{
    (void)buf;

    uart_send_str("Item nr. name size [width x heigh]\r\n");

    char str_buf[MAX_STR_LEN];
    uint8_t num = 0u;
    do {
        const storage_item_head_t* item = storage_get_item(num);
        if (NULL == item) {
            break;
        }

        if (jpeg_info((uint8_t*)(&item[1u]), item->len) == true) {
            snprintf(str_buf, sizeof(str_buf), "%u. %s %lu %ux%u\r\n", num, item->name, item->len, cinfo.image_width, cinfo.image_height);
        } else {
            snprintf(str_buf, sizeof(str_buf), "%u. %s %lu not jpeg\r\n", num, item->name, item->len);
        }

        uart_send_str(str_buf);
    } while (++num != 0);
}

static void _freq_cmd(uint8_t* buf)
{
    char str_buf[MAX_STR_LEN];
    const char* resp = "";

    if (0 != *buf) {
        uint32_t param = my_strtoul(&buf);
        if (0 != *buf) {
            resp = "Invalid value\r\n";
        } else if (pwm_set_freq(param) == false) {
            resp = "Value out of range\r\n";
        } else {
            snprintf(str_buf, sizeof(str_buf), "New freq: %luHz\r\n", pwm_get_freq());
            resp = str_buf;
        }
    } else {
        snprintf(str_buf, sizeof(str_buf), "Freq: %luHz\r\n", pwm_get_freq());
        resp = str_buf;
    }
    uart_send_str(resp);
}

static void _duty_cmd(uint8_t* buf)
{
    char str_buf[MAX_STR_LEN];
    const char* resp = "";

    if (0 != *buf) {
        uint32_t param = my_strtoul(&buf);
        if (0 != *buf) {
            resp = "Invalid value\r\n";
        } else if (pwm_set_duty(param) == false) {
            resp = "Value out of range\r\n";
        } else {
            snprintf(str_buf, sizeof(str_buf), "New duty: %lu%%\r\n", pwm_get_duty());
            resp = str_buf;
        }
    } else {
        snprintf(str_buf, sizeof(str_buf), "Duty: %lu%%\r\n", pwm_get_duty());
        resp = str_buf;
    }
    uart_send_str(resp);
}

static void _invert_out(uint8_t* buf)
{
    (void)buf;

    uart_send_str("Invert PWM out\r\n");
    pwm_invert_out();
}

static void _set_update(uint8_t* buf)
{
    (void)buf;

    if (pwm_toggle_immediate_update() == true) {
        uart_send_str("New PWM settings applied immediately\r\n");
    } else {
        uart_send_str("New PWM settings applied after reset\r\n");
    }
}

static void _print_cmds(uint8_t* buf)
{
    (void)buf;

    uint32_t i = 0u;
    for (; i < ARRAYSIZE(commands) - 1u ; i++) {
        uart_send((const uint8_t*)commands[i].d.name, 4u);
        uart_send_str(", ");
    }
    uart_send((const uint8_t*)commands[i].d.name, 4u);
    uart_send_str("\r\n");
}

static void _help_cmd(uint8_t* buf)
{
    (void)buf;

    char str_buf[MAX_STR_LEN];
    for (uint32_t i = 0u; i < ARRAYSIZE(commands); i++) {
        uart_send((const uint8_t*)commands[i].d.name, 4u);
        snprintf(str_buf, sizeof(str_buf), " - %s\r\n", commands[i].description);
        uart_send_str(str_buf);
    }
}

static void _trim_end(uint8_t* line)
{
    uint8_t* end = line;
    for (; 0 != *end; end++);
    while (--end  >= line) {
        if (is_space(*end) == true) {
            *end = 0;
        } else {
            break;
        }
    }
}

static void _handle_command(uint8_t* line)
{
    if ((0u == line[4u]) || (true == is_space(line[4u]))) {
        uint32_t cmd_id = *((uint32_t*)line);
        for (uint32_t i = 0u; i < ARRAYSIZE(commands); i++) {
            if (commands[i].d.id == cmd_id) {
                commands[i].handler(&line[4]);
                return;
            }
        }
    }
    uart_send_str("Invalid command, try help\r\n");
}

static void _check_uart(void)
{
    line_buf_t line;

    if (uart_get_line(&line) == true) {
        _trim_end(line.buf);
        if (0u != line.buf[0]){
            _handle_command(line.buf);
        }
        command_prompt();
    }
}

static void _check_touch(void)
{
    touch_cmd_t cmd = touch_get_command();
    if (SWIPED_LEFT == cmd) {
        _show_next(NULL);
    } else if (SWIPED_RIGHT == cmd) {
        _show_prev(NULL);
    }
}

// -- Public Interface --
void command_prompt(void)
{
    uart_send_byte('>');
}

void command_check(void)
{
    _check_uart();
    _check_touch();
}

bool command_exec(cmd_exec_t cmd)
{
    (void)cmd;
    // only default action for now
    return _display_image();
}