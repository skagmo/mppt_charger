#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#define CHAR_WIDTH			5
#define CHAR_BIG_WIDTH		7
#define LCD_WIDTH			128
#define LCD_X_OFFSET		0
#define LCD_LINES			8
#define CMD_DISPLAY_ON		0xAF
#define CMD_DISP_START_LINE 0x40
#define CMD_PAGE			0xB0
#define CMD_COLUMN_UPPER	0x10
#define CMD_COLUMN_LOWER	0x00
#define CMD_ADC_NORMAL		0xA0
#define CMD_ADC_REV			0xA1
#define CMD_ALLPTS_NORMAL	0xA4
#define CMD_BIAS_7			0xA3
#define CMD_RMW				0xE0
#define CMD_SHL_NORMAL		0xC0
#define CMD_SHL_FLIP		0xC8
#define CMD_POWER_CONTROL	0x28
#define CMD_RESISTOR_RATIO  0x20
#define CMD_VOLUME_FIRST	0x81
#define CMD_VOLUME_SECOND	0x00

// Skagmo Electronics logo converted with the dot factory. Bit layout: ColumnMajor, lsbfirst.
extern const uint8_t se_logo[];
extern const uint8_t lcd_font[][5];
// Big number font from arduino GLCD library
extern const uint8_t lcd_font_big[];

void lcd_bitmap(const uint8_t* gfx, uint8_t x_size, uint8_t line_size);
void lcd_clear(void);
void lcd_put(uint8_t c);
void lcd_put_large(uint8_t c);
void lcd_puts_large(const char* s);
void lcd_puts(const char* s);
void lcd_cursor(uint8_t x, uint8_t line);
void delay_ms(uint16_t n);
void lcd_init(void);
void lcd_refresh(void);

#endif