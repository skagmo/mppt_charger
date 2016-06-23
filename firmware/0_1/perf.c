#include "perf.h"

#include <xc.h>
#include <stdint.h>
#include "platform.h"

void pwm_init(void){
	// Set up T2
	PR2 = 31;	// 4*(31+1) = 128, 125 KHz
	T2CONbits.T2OUTPS = 0;	// 1:1 postscale
	T2CONbits.T2CKPS = 0;	// 1:1 prescale
	T2CONbits.TMR2ON = 1;

	// Set up CCP
	CCP1CONbits.CCP1M = 0b1100; // P1A active high, P1B active low
	CCP1CONbits.P1M = 0b10; // Half bridge with P1A and P1B as outputs

	// Deadband
	PWM1CON = 1; // Units of 0.25 uS (1 / 16 MHz / 4)
}

void pwm_deinit(void){
	CCP1CONbits.CCP1M = 0; // Turn of CCP-module
	PIC_BUCK_HI = PIC_BUCK_LO = 0;
}

void pwm_set(uint8_t level){
	CCPR1L = (level >> 2);
	CCP1CONbits.DC1B = level & 0b11;
}

void ad_init(void){
	// Enable AN5-AN9 input buffer
	ANSEL    = 0b11100000;
	ANSELH   = 0b00000011;
	ADCON1bits.PVCFG = 0b10; // Positive voltage from FVR
	ADCON1bits.NVCFG = 0; // Negative voltage from Vss
	ADCON2   = 0b10101110;  // Right justify, 20TAD, FOSC/64

	// Voltage reference
	VREFCON0 = 0b10100000;  // FVR1EN, 2.048V
  	ADCON0bits.ADON = 1;  // ADC on
}

int ad_read(char channel){
 	ADCON0 = (channel<<2) | 0b11;
	while (ADCON0&0b10);
	return ((ADRESH<<8) + ADRESL);
}