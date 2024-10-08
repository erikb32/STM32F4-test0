#include <stdint.h>
#include <stdio.h>

#include "touch.h"
#include "lcd.h"
#include "uart.h"
#include "stm32412g_discovery.h"
#include "stm32412g_discovery_ts.h"

#define TS_CHECK_PERIOD 50u

static uint8_t _ts_init_status;
static uint16_t _ts_x0 = 0xFFFFu;
static touch_cmd_t _pending_command = NO_ACTION;

void touch_init(void)
{
    _ts_init_status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
}

void touch_info(void)
{
    // Note: Sometimes touch panel does not communicate, reset pin seems not helping, only full power off.
    // Maybe try I2C bus manual reset or maybe better I2C driver is needed
    if (TS_OK != _ts_init_status) {
        char str_buf[64u];
        snprintf(str_buf, sizeof(str_buf), "Touch panel fail: %u, try full power off\r\n", _ts_init_status);
        uart_send_str(str_buf);
    }
}

void touch_poll(void)
{
    static uint32_t last_tick = 0u;

    if (TS_OK != _ts_init_status) {
        return;
    }

    uint32_t tick = HAL_GetTick();
    if ((tick - last_tick) > TS_CHECK_PERIOD) {
        last_tick = tick;
        TS_StateTypeDef TS_State = {0};

        if (BSP_TS_GetState(&TS_State) == TS_OK) {
            if (TS_State.touchDetected) {
                if (0xFFFFu == _ts_x0) {
                    _ts_x0 = TS_State.touchX[0];
                } else {
                    uint16_t x1 = TS_State.touchX[0];
                    // uint16_t y1 = TS_State.touchY[0];

                    if (_ts_x0 > x1) {
                        _pending_command = SWIPED_LEFT;
                    } else if (_ts_x0 < x1) {
                        _pending_command = SWIPED_RIGHT;
                    }
                    _ts_x0 = 0xFFFFu;
                }
            }
        }
    }
}

touch_cmd_t touch_get_command(void)
{
    touch_cmd_t ret = _pending_command;
    _pending_command = NO_ACTION;
    return ret;
}

// Currently unused
void EXTI9_5_IRQHandler(void)
{
   HAL_GPIO_EXTI_IRQHandler(TS_INT_PIN);
}
