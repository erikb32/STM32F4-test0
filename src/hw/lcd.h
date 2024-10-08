#ifndef _LCD_H_
#define _LCD_H_

#include "stm32412g_discovery_lcd.h"

#define LCD_HEIGHT 240u
#define LCD_WIDTH  240u

#define lcd_off()   BSP_LCD_DisplayOff()
#define lcd_on()    BSP_LCD_DisplayOn()

void lcd_init(void);
void lcd_clear(void);
void lcd_info(uint8_t* str);

#endif // _LCD_H_
