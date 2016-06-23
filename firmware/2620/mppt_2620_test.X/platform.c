#include "platform.h"

#include <p18f2620.h>

void platform_init(){
	// Oscillator setup
	OSCCONbits.IRCF = 0b111;	// 8MHz int osc.
	OSCCONbits.OSTS = 0;		// Internal osc.
	OSCCONbits.SCS = 0b10;		// Int. osc.
    OSCTUNEbits.PLLEN = 0;		// No PLL

	// Port setup
    TRISA   = 0b11111111;
    TRISB   = 0b11111000;
    TRISC   = 0b11010111;

	// UART_TX RC6, UART_RX RC7
}