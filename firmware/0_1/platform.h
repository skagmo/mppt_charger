#ifndef PLATFORM_H
#define	PLATFORM_H

#define _XTAL_FREQ 16000000

// Digital pin definitions
#define PIC_BUCK_HI			LATCbits.LATC5
#define PIC_BUCK_LO			LATCbits.LATC4

// Analog pin definitions
#define AD_PV_I			5
#define AD_BAT_IN_I		6
#define AD_PV_V			7
#define AD_BAT_V		8
#define AD_BAT_OUT_I	9

#pragma config FOSC = IRC       // Oscillator Selection bits (Internal RC oscillator)
#pragma config PLLEN = OFF      // 4 X PLL Enable bit (PLL is under software control)
#pragma config PCLKEN = ON      // Primary Clock Enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 19        // Brown Out Reset Voltage bits (VBOR set to 1.9 V nominal)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bit (WDT is controlled by SWDTEN bit of the WDTCON register)
#pragma config WDTPS = 32    // Watchdog Timer Postscale Select bits (1:32768)
#pragma config HFOFST = OFF     // HFINTOSC Fast Start-up bit (The system clock is held off until the HFINTOSC is stable.)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RA3 input pin enabled; MCLR disabled)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

void general_init();

#endif
