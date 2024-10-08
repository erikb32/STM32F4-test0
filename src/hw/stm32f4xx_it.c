
#include <stdint.h>
#include "utils.h"

#include "stm32f4xx_hal.h"

__attribute__((noreturn)) static void _halt_blinky(uint32_t ms)
{
    // GPIOE, pin 2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    // Delay after enabling GPIOE clock

    GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD2);
    GPIOE->OTYPER &= ~(GPIO_OTYPER_OT2);

    uint32_t tmp = GPIOE->MODER;
    tmp &= ~(GPIO_MODER_MODER2);
    tmp |= GPIO_MODER_MODER2_0;
    GPIOE->MODER = tmp;

    while (1) {
        delay_ms(ms);
        GPIOE->BSRR = GPIO_PIN_2;
        delay_ms(ms);
        GPIOE->BSRR = (uint32_t)GPIO_PIN_2 << 16U;
    }
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    _halt_blinky(128u);
}

void MemManage_Handler(void)
{
    _halt_blinky(512u);
}

void BusFault_Handler(void)
{
    _halt_blinky(512u);
}

void UsageFault_Handler(void)
{
    _halt_blinky(512u);
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
