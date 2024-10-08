#include <stdint.h>

#include "led.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

#define LED_BLINKY_PERIOD   500u

#define LED_BLINKY_IO_BASE  GPIOA
#define LED_BLINKY_PIN      GPIO_PIN_5

void led_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init_structure;
    gpio_init_structure.Pin = LED_BLINKY_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_BLINKY_IO_BASE, &gpio_init_structure);

    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_ORANGE);
    BSP_LED_Init(LED_RED);
    BSP_LED_Init(LED_BLUE);
}

void led_blinky(void)
{
    static uint32_t last_tick = 0u;

    uint32_t tick = HAL_GetTick();
    if ((tick - last_tick) > LED_BLINKY_PERIOD) {
        last_tick = tick;
        HAL_GPIO_TogglePin(LED_BLINKY_IO_BASE, LED_BLINKY_PIN);
    }
}
