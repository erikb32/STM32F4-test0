#include <stdint.h>

#include "lcd.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

void lcd_init(void)
{
    BSP_LCD_Init();
    BSP_LCD_DisplayOff();
    lcd_clear();
}

void lcd_clear(void)
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);
}

void lcd_info(uint8_t* str)
{
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, 112, str, CENTER_MODE);
}
