#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <p18f2620.h>
#include "platform.h"

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

uint8_t lcd_x;
uint8_t lcd_line;
uint8_t lcd_buf[1024];

// Skagmo Electronics logo converted with the dot factory. Bit layout: ColumnMajor, lsbfirst.
const uint8_t se_logo[] = {
	0x00, 0xC0, 0xF0, 0x78, 0x1C, 0x0C, 0x0E, 0x06, 0x06, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x83, 0xC3, 0xE3, 0x73, 0x3B, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF8, 0x1C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1C, 0x3C, 0xF8, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
	0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0xFC, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xC7, 0x07, 0x0E, 0x0E, 0x0C, 0x1C, 0x1C, 0x1C, 0x18, 0x38, 0x78, 0xF0, 0xF0, 0xC0, 0x00, 0x00, 0xFF, 0xFF, 0xF0, 0x70, 0x78, 0xFC, 0xEE, 0xC7, 0x03, 0x03, 0x01, 0x00, 0x8C, 0xCF, 0xEF, 0xE3, 0x61, 0x61, 0x61, 0x61, 0x33, 0xFF, 0xFF, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0x07, 0x03, 0x01, 0x01, 0x01, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x03, 0x01, 0x01, 0x01, 0x01, 0x07, 0xFF, 0xFF, 0x07, 0x03, 0x01, 0x01, 0x01, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x0F, 0x03, 0x01, 0x01, 0x01, 0x01, 0x03, 0x07, 0xFF, 0xFE,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 0x1C, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x1E, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1E, 0x1C, 0x00, 0x0F, 0x1F, 0x1D, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x0E, 0x0F, 0x1F, 0x18, 0x18, 0x00, 0xC3, 0xCF, 0xCE, 0x1C, 0x1C, 0x18, 0x18, 0x1C, 0x8E, 0xF7, 0xFF, 0x7F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1E, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x1E, 0x0F, 0x07,
	0xFF, 0x7F, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xE0, 0x60, 0x60, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x60, 0x60, 0xE0, 0xC0, 0xC0, 0x00, 0x40, 0xF8, 0xF8, 0x40, 0x40, 0x00, 0x80, 0xC0, 0xE0, 0x61, 0x63, 0x63, 0x03, 0xC3, 0xC3, 0xE3, 0x63, 0x63, 0xE1, 0xC0, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0xE0, 0x60, 0x60, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0xDC, 0xDC, 0x00, 0x00, 0xC0, 0xC0, 0xE0, 0x60, 0x60, 0xE0, 0xC0, 0x80, 0x00, 0x80, 0xC0, 0xC0, 0x60, 0x60, 0xE0, 0xC0, 0xC0, 0x80,
	0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xB8, 0x9C, 0x8E, 0x87, 0x83, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xE0, 0x70, 0x38, 0x3E, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0xCC, 0xCC, 0x8C, 0x8C, 0xCC, 0xEF, 0x6F, 0x00, 0x3F, 0x7F, 0xE1, 0xC0, 0x80, 0x80, 0xC0, 0xF3, 0x73, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xE1, 0xC0, 0x80, 0x80, 0xC0, 0xE1, 0x7F, 0x3F, 0x00, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0xE1, 0xC0, 0x80, 0x80, 0xC0, 0xF3, 0x73, 0x00, 0x73, 0xF7, 0xC6, 0x8E, 0x8C, 0x8C, 0xDC, 0xF9, 0x79,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
};

const uint8_t lcd_font[][5] = {
	{0x00,0x00,0x00,0x00,0x00},   //   0x20 32
	{0x00,0x00,0x6f,0x00,0x00},   // ! 0x21 33
	{0x00,0x07,0x00,0x07,0x00},   // " 0x22 34
	{0x14,0x7f,0x14,0x7f,0x14},   // # 0x23 35
	{0x00,0x07,0x04,0x1e,0x00},   // $ 0x24 36
	{0x23,0x13,0x08,0x64,0x62},   // % 0x25 37
	{0x36,0x49,0x56,0x20,0x50},   // & 0x26 38
	{0x00,0x00,0x07,0x00,0x00},   // ' 0x27 39
	{0x00,0x1c,0x22,0x41,0x00},   // ( 0x28 40
	{0x00,0x41,0x22,0x1c,0x00},   // ) 0x29 41
	{0x14,0x08,0x3e,0x08,0x14},   // * 0x2a 42
	{0x08,0x08,0x3e,0x08,0x08},   // + 0x2b 43
	{0x00,0x50,0x30,0x00,0x00},   // , 0x2c 44
	{0x08,0x08,0x08,0x08,0x08},   // - 0x2d 45
	{0x00,0x60,0x60,0x00,0x00},   // . 0x2e 46
	{0x20,0x10,0x08,0x04,0x02},   // / 0x2f 47
	{0x3e,0x51,0x49,0x45,0x3e},   // 0 0x30 48
	{0x00,0x42,0x7f,0x40,0x00},   // 1 0x31 49
	{0x42,0x61,0x51,0x49,0x46},   // 2 0x32 50
	{0x21,0x41,0x45,0x4b,0x31},   // 3 0x33 51
	{0x18,0x14,0x12,0x7f,0x10},   // 4 0x34 52
	{0x27,0x45,0x45,0x45,0x39},   // 5 0x35 53
	{0x3c,0x4a,0x49,0x49,0x30},   // 6 0x36 54
	{0x01,0x71,0x09,0x05,0x03},   // 7 0x37 55
	{0x36,0x49,0x49,0x49,0x36},   // 8 0x38 56
	{0x06,0x49,0x49,0x29,0x1e},   // 9 0x39 57
	{0x00,0x36,0x36,0x00,0x00},   // : 0x3a 58
	{0x00,0x56,0x36,0x00,0x00},   // ; 0x3b 59
	{0x08,0x14,0x22,0x41,0x00},   // < 0x3c 60
	{0x14,0x14,0x14,0x14,0x14},   // = 0x3d 61
	{0x00,0x41,0x22,0x14,0x08},   // > 0x3e 62
	{0x02,0x01,0x51,0x09,0x06},   // ? 0x3f 63
	{0x3e,0x41,0x5d,0x49,0x4e},   // @ 0x40 64
	{0x7e,0x09,0x09,0x09,0x7e},   // A 0x41 65
	{0x7f,0x49,0x49,0x49,0x36},   // B 0x42 66
	{0x3e,0x41,0x41,0x41,0x22},   // C 0x43 67
	{0x7f,0x41,0x41,0x41,0x3e},   // D 0x44 68
	{0x7f,0x49,0x49,0x49,0x41},   // E 0x45 69
	{0x7f,0x09,0x09,0x09,0x01},   // F 0x46 70
	{0x3e,0x41,0x49,0x49,0x7a},   // G 0x47 71
	{0x7f,0x08,0x08,0x08,0x7f},   // H 0x48 72
	{0x00,0x41,0x7f,0x41,0x00},   // I 0x49 73
	{0x20,0x40,0x41,0x3f,0x01},   // J 0x4a 74
	{0x7f,0x08,0x14,0x22,0x41},   // K 0x4b 75
	{0x7f,0x40,0x40,0x40,0x40},   // L 0x4c 76
	{0x7f,0x02,0x0c,0x02,0x7f},   // M 0x4d 77
	{0x7f,0x04,0x08,0x10,0x7f},   // N 0x4e 78
	{0x3e,0x41,0x41,0x41,0x3e},   // O 0x4f 79
	{0x7f,0x09,0x09,0x09,0x06},   // P 0x50 80
	{0x3e,0x41,0x51,0x21,0x5e},   // Q 0x51 81
	{0x7f,0x09,0x19,0x29,0x46},   // R 0x52 82
	{0x46,0x49,0x49,0x49,0x31},   // S 0x53 83
	{0x01,0x01,0x7f,0x01,0x01},   // T 0x54 84
	{0x3f,0x40,0x40,0x40,0x3f},   // U 0x55 85
	{0x0f,0x30,0x40,0x30,0x0f},   // V 0x56 86
	{0x3f,0x40,0x30,0x40,0x3f},   // W 0x57 87
	{0x63,0x14,0x08,0x14,0x63},   // X 0x58 88
	{0x07,0x08,0x70,0x08,0x07},   // Y 0x59 89
	{0x61,0x51,0x49,0x45,0x43},   // Z 0x5a 90
	{0x3c,0x4a,0x49,0x29,0x1e},   // [ 0x5b 91
	{0x02,0x04,0x08,0x10,0x20},   // \ 0x5c 92
	{0x00,0x41,0x7f,0x00,0x00},   // ] 0x5d 93
	{0x04,0x02,0x01,0x02,0x04},   // ^ 0x5e 94
	{0x40,0x40,0x40,0x40,0x40},   // _ 0x5f 95
	{0x00,0x00,0x03,0x04,0x00},   // ` 0x60 96
	{0x20,0x54,0x54,0x54,0x78},   // a 0x61 97
	{0x7f,0x48,0x44,0x44,0x38},   // b 0x62 98
	{0x38,0x44,0x44,0x44,0x20},   // c 0x63 99
	{0x38,0x44,0x44,0x48,0x7f},   // d 0x64 100
	{0x38,0x54,0x54,0x54,0x18},   // e 0x65 101
	{0x08,0x7e,0x09,0x01,0x02},   // f 0x66 102
	{0x0c,0x52,0x52,0x52,0x3e},   // g 0x67 103
	{0x7f,0x08,0x04,0x04,0x78},   // h 0x68 104
	{0x00,0x44,0x7d,0x40,0x00},   // i 0x69 105
	{0x20,0x40,0x44,0x3d,0x00},   // j 0x6a 106
	{0x00,0x7f,0x10,0x28,0x44},   // k 0x6b 107
	{0x00,0x41,0x7f,0x40,0x00},   // l 0x6c 108
	{0x7c,0x04,0x18,0x04,0x78},   // m 0x6d 109
	{0x7c,0x08,0x04,0x04,0x78},   // n 0x6e 110
	{0x38,0x44,0x44,0x44,0x38},   // o 0x6f 111
	{0x7c,0x14,0x14,0x14,0x08},   // p 0x70 112
	{0x08,0x14,0x14,0x18,0x7c},   // q 0x71 113
	{0x7c,0x08,0x04,0x04,0x08},   // r 0x72 114
	{0x48,0x54,0x54,0x54,0x20},   // s 0x73 115
	{0x04,0x3f,0x44,0x40,0x20},   // t 0x74 116
	{0x3c,0x40,0x40,0x20,0x7c},   // u 0x75 117
	{0x1c,0x20,0x40,0x20,0x1c},   // v 0x76 118
	{0x3c,0x40,0x30,0x40,0x3c},   // w 0x77 119
	{0x44,0x28,0x10,0x28,0x44},   // x 0x78 120
	{0x0c,0x50,0x50,0x50,0x3c},   // y 0x79 121
	{0x44,0x64,0x54,0x4c,0x44},   // z 0x7a 122
	{0x00,0x08,0x36,0x41,0x41},   // { 0x7b 123
	{0x00,0x00,0x7f,0x00,0x00},   // | 0x7c 124
	{0x41,0x41,0x36,0x08,0x00},   // } 0x7d 125
	{0x04,0x02,0x04,0x08,0x04},   // ~ 0x7e 126
	{0x7f,0x6b,0x6b,0x6b,0x7f},   //  0x7f 127
};

// Big number font from arduino GLCD library
const uint8_t lcd_font_big[] = {
// char '+'
0x00, 0x80, 0x80, 0xe0, 0xe0, 0x80, 0x80,
0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,

// char ','
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x58, 0x38, 0x00, 0x00,

// char '-'
0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// char '.'
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00,

// char '/'
0x00, 0x00, 0x00, 0xc0, 0xf0, 0x3c, 0x0f,
0x00, 0x3c, 0x0f, 0x03, 0x00, 0x00, 0x00,

// char '0'
0xfc, 0xfe, 0x03, 0xe1, 0x1b, 0xfe, 0xfc,
0x0f, 0x1f, 0x36, 0x21, 0x30, 0x1f, 0x0f,

// char '1'
0x04, 0x04, 0x06, 0xff, 0xff, 0x00, 0x00,
0x20, 0x20, 0x20, 0x3f, 0x3f, 0x20, 0x20,

// char '2'
0x0c, 0x0e, 0x03, 0x01, 0x81, 0xfe, 0x7c,
0x38, 0x3c, 0x26, 0x23, 0x21, 0x20, 0x20,

// char '3'
0x0c, 0x0e, 0x43, 0x41, 0x43, 0xfe, 0xbc,
0x0c, 0x1c, 0x30, 0x20, 0x30, 0x1f, 0x0f,

// char '4'
0x00, 0xe0, 0xfc, 0x1f, 0x83, 0x80, 0x00,
0x0f, 0x0f, 0x08, 0x08, 0x3f, 0x3f, 0x08,

// char '5'
0x3f, 0x3f, 0x21, 0x21, 0x61, 0xe1, 0x81,
0x0c, 0x1c, 0x30, 0x20, 0x30, 0x3f, 0x0f,

// char '6'
0xe0, 0xf8, 0x5c, 0x46, 0xc3, 0xc1, 0x01,
0x0f, 0x1f, 0x30, 0x20, 0x30, 0x3f, 0x0f,

// char '7'
0x01, 0x01, 0x01, 0x81, 0xf1, 0x7f, 0x0f,
0x00, 0x00, 0x3c, 0x3f, 0x03, 0x00, 0x00,

// char '8'
0x1c, 0xbe, 0xe3, 0x41, 0xe3, 0xbe, 0x1c,
0x0f, 0x1f, 0x30, 0x20, 0x30, 0x1f, 0x0f,

// char '9'
0x3c, 0x7e, 0xc3, 0x81, 0x81, 0xfe, 0xfc,
0x20, 0x30, 0x38, 0x0c, 0x07, 0x03, 0x00,

// char ':'
0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00,

};

void lcd_bitmap(uint8_t* gfx, uint8_t x_size, uint8_t line_size){
	uint8_t x, line;
	for (line=0; line<line_size; line++){
		for (x=0; x<x_size; x++){
			lcd_buf[x+lcd_x+((line+lcd_line)*LCD_WIDTH)] = gfx[x+(line*x_size)];
		}
	}
}

#include "string.h"
void lcd_clear(void){
	memset (lcd_buf, 0, (LCD_WIDTH*LCD_LINES));
}

void spi(char data){
    (void)SSPBUF;
    SSPBUF = data;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void lcd_put(uint8_t c){
	uint8_t j;
	if (lcd_x > (LCD_WIDTH-CHAR_WIDTH+1)){
		lcd_x = 0;
		lcd_line++;
	}
	for (j=0; j<5; j++){
		lcd_buf[lcd_x+(lcd_line*LCD_WIDTH)] = lcd_font[c-0x20][j];
		lcd_x++;
	}
	lcd_buf[lcd_x+(lcd_line*LCD_WIDTH)] = 0;
	lcd_x++;
}

void lcd_put_large(uint8_t c){
	uint8_t j;

	// Check boundaries
	if (lcd_x > (LCD_WIDTH-CHAR_BIG_WIDTH)){
		lcd_x = 0;
		lcd_line+=2;
	}
	if (lcd_line < 1) lcd_line = 1;
	else if (lcd_line > 7) lcd_line = 7;

	// Upper line
	for (j=0; j<CHAR_BIG_WIDTH; j++){
		lcd_buf[lcd_x+((lcd_line-1)*LCD_WIDTH)] = lcd_font_big[((c-0x2b)*CHAR_BIG_WIDTH*2)+j];
		lcd_x++;
	}
	lcd_buf[lcd_x+((lcd_line-1)*LCD_WIDTH)] = 0;
	lcd_x -= CHAR_BIG_WIDTH;

	// Lower line
	for (j=CHAR_BIG_WIDTH; j<(CHAR_BIG_WIDTH*2); j++){
		lcd_buf[lcd_x+(lcd_line*LCD_WIDTH)] = lcd_font_big[((c-0x2b)*CHAR_BIG_WIDTH*2)+j];
		lcd_x++;
	}
	lcd_buf[lcd_x+(lcd_line*LCD_WIDTH)] = 0;
	lcd_x++;
}

void lcd_puts_large(char* s){
	while (*s) lcd_put_large(*(s++));
}

void lcd_puts(char* s){
	while (*s) lcd_put(*(s++));
}

void lcd_cursor(uint8_t x, uint8_t line){
	lcd_x = x;
	lcd_line = line;
}

void delay_ms(uint16_t n){
	uint16_t j;
	for (j=0;j<n;j++) __delay_ms(1);
}

void lcd_init(void){
	LCD_CS = 0;
	LCD_RESET = 0;
	delay_ms(1);
	LCD_RESET = 1;
	delay_ms(1);
	LCD_DATA_SEL = 0;
	LCD_CS = 0;
	spi(CMD_BIAS_7);				// Bias 1/7
	spi(CMD_ADC_REV);				// X-direction flip
	spi(CMD_SHL_FLIP);				// Y-direction flip
	spi(CMD_ALLPTS_NORMAL);
	spi(CMD_DISP_START_LINE);		// Start line 0
	spi(CMD_POWER_CONTROL | 0x4);	// VC on
	delay_ms(50);
	spi(CMD_POWER_CONTROL | 0x6);	// VR on
	delay_ms(50);
	spi(CMD_POWER_CONTROL | 0x7);	// VF on
	delay_ms(50);
	spi(CMD_RESISTOR_RATIO | 3);	// Set operating voltage
    spi(CMD_VOLUME_FIRST);
    spi(CMD_VOLUME_SECOND | (33 & 0x3f));	// Set contrast to 28 for Zolen, 33 for other
	spi(CMD_DISPLAY_ON);				// Enable display
	LCD_CS = 1;
}



void lcd_refresh(){
	uint8_t x, y;
	for (y=0; y<8; y++){
		LCD_DATA_SEL = 0;
		LCD_CS = 0;
		spi(CMD_PAGE | y);
		spi(CMD_COLUMN_LOWER|LCD_X_OFFSET);
		spi(CMD_COLUMN_UPPER);
		spi(CMD_RMW);
		LCD_DATA_SEL = 1;
		for(x=0; x<LCD_WIDTH; x++) spi(lcd_buf[x+(y*LCD_WIDTH)]);
		LCD_CS = 1;
	}
}

int main(int argc, char** argv) {
	platform_init();
	
	// SPI config
	SSPCON1bits.SSPM = 0; // 0=osc/4, 1=osc/16, 2 = osc/64
	SSPSTATbits.CKE = 1;
	SSPCON1bits.SSPEN = 1;

	lcd_init();
	lcd_clear();
	lcd_bitmap(se_logo, 127, 6);
	lcd_cursor(0, 6);
	lcd_puts("MPPT charger");
	lcd_cursor(0, 7);
	lcd_puts("HW 0.2, SW 0.2");
	lcd_refresh();
	delay_ms(2000);

	// REaltime, stats
	#define LCD_MODE_MEASURE 0
	#define LCD_MODE_PLOT 1
	#define LCD_MODE_SETTINGS 2
	uint8_t lcd_mode = LCD_MODE_MEASURE;

	while(1){
		lcd_clear();
		// If timed out:
		// Return to LCD_MODE_INFO
		// Turn off backlight if set to auto
		//
		switch (lcd_mode){
			case LCD_MODE_MEASURE:
				lcd_cursor(20, 0); lcd_puts("- Measurement -");
				lcd_cursor(0, 1); lcd_puts("State"); lcd_cursor(64, 1); lcd_puts("CHG");
				lcd_cursor(0, 2); lcd_puts("PV"); lcd_cursor(64, 2); lcd_puts("18.51"); lcd_put('V');
				lcd_cursor(0, 3); lcd_puts("Battery"); lcd_cursor(64, 3); lcd_puts("12.54"); lcd_put('V');

				lcd_cursor(0, 4); lcd_puts("Charge");
				lcd_cursor(0, 5); lcd_puts(" 0.81A"); lcd_cursor(64, 5); lcd_puts_large("7.5"); lcd_put('W');
				
				lcd_cursor(0, 6); lcd_puts("Load");
				lcd_cursor(0, 7); lcd_puts(" 22.27A"); lcd_cursor(64, 7); lcd_puts_large("127.3"); lcd_put('W');
				break;
			case LCD_MODE_PLOT:
				// To bat.    124Wh
				// From bat.  1215Wh
				// Effici.    89.3%
				// plot vs time! Log 24 hours charge and load, interval depends
				break;
			case LCD_MODE_SETTINGS:
				lcd_puts("OUT_1 OFF");
				lcd_puts("OUT_2 OFF");
				lcd_puts("BACKLIGHT OFF"); // OFF/ON/AUTO
				lcd_puts("RESET_STATS");
				lcd_puts("PLOT_TYPE"); // CHG_W, LOAD_W
				lcd_puts("PLOT_TIME"); // 1-24 hours
		}
		lcd_refresh();
		delay_ms(500);
	}
  
	return (EXIT_SUCCESS);
}

