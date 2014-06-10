#include "platform.h"

#include <xc.h>

void general_init(){
	// Oscillator config
	OSCCON = 0b01110010;  // Int. OSC at 16 MHz
	OSCTUNEbits.PLLEN = 0;
	WDTCON = 0;

	// IO-settings
	TRISA = 0b11111111;
	TRISB = 0b00101111;
	TRISC = 0b11001110;
}
