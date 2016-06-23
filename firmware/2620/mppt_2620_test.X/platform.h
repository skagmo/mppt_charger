#ifndef PLATFORM_H
#define PLATFORM_H

#pragma config OSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
//#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config WDT = OFF, LVP = OFF, PBADEN = 0, MCLRE = 0     // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#define _XTAL_FREQ 8000000
#define LCD_CS       LATBbits.LATB0
#define LCD_RESET    LATBbits.LATB1
#define LCD_DATA_SEL LATBbits.LATB2 // A0 Low means control

void platform_init();

#endif