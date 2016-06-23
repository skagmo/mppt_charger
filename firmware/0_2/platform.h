#ifndef PLATFORM_02_H
#define	PLATFORM_02_H

#include <stdint.h>

#define _XTAL_FREQ 64000000

// Digital pin definitions
#define PIC_BUCK_HI   LATCbits.LATC2
#define PIC_BUCK_LO   LATBbits.LATB2
#define AUX1_EN       LATCbits.LATC0
#define AUX2_EN       LATCbits.LATC1
#define LCD_CS        LATBbits.LATB4
#define LCD_RESET     LATBbits.LATB1
#define LCD_DATA_SEL  LATBbits.LATB0
#define LCD_BL        LATBbits.LATB3
	
// Analog pin definitions
#define AD_PV_I			0
#define AD_PV_V			1
#define AD_BAT_IN_I		2
#define AD_BAT_V		3
#define AD_BAT_OUT_I	4

#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = ON     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = OFF   // Primary clock enable bit (Primary clock can be disabled by software)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTC6  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTC0   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = INTMCLR  // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

void general_init(void);
void delay_ms(uint16_t n);
void set_outputs(void);

#endif
