#include "platform.h"

#include <xc.h>
#include "config.h"

void general_init(void){
	// Oscillator config
	OSCCONbits.IRCF = 0b111; // 110 = 8 MHz, 111 = 16 MHz
	WDTCON = 0;
	OSCTUNEbits.PLLEN = 1;

	// Outputs
	TRISBbits.TRISB2 = 0;	// P1B
	TRISCbits.TRISC0 = 0;	// AUX1
	TRISCbits.TRISC1 = 0;	// AUX2
	TRISCbits.TRISC2 = 0;	// P1A
	TRISBbits.TRISB4 = 0;	// LCD_CS
	TRISBbits.TRISB1 = 0;	// LCD_RESET
	TRISBbits.TRISB0 = 0;	// LCD_DATA_SEL
	TRISCbits.TRISC3 = 0;	// LCD_CLOCK
	TRISCbits.TRISC5 = 0;	// LCD_SI
	TRISBbits.TRISB3 = 0;   // LCD_BL
}

void delay_ms(uint16_t n){
	uint16_t j;
	for (j=0;j<n;j++) __delay_ms(1);
}
