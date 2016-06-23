#include "perf.h"

#include <xc.h>
#include <stdint.h>
#include "platform.h"

void t0_init(void){
	// Set up T0
	T0CONbits.TMR0ON = 0;
	T0CONbits.T08BIT = 0;   // 16-bit mode
	T0CONbits.T0CS = 0;     // Internal instruction clock
	T0CONbits.PSA = 0;      // Prescaller on
	T0CONbits.T0PS = 0b110; // 1:128 prescaler
	T0CONbits.TMR0ON = 1;
}

void spi_init(void){
	// SPI config
	SSP1CON1bits.SSPM = 1; // 0=osc/4, 1=osc/16, 2 = osc/64
	SSP1STATbits.CKE = 1;
	SSP1CON1bits.SSPEN = 1;
}

void spi(char data){
    (void)SSP1BUF;
    SSP1BUF = data;
    while(PIR1bits.SSP1IF == 0);
    PIR1bits.SSP1IF = 0;
}

void pwm_init(void){
	// Set up T2
	PR2 = 127;	// 4*(31+1) = 128, 125 KHz
	T2CONbits.T2OUTPS = 0;	// 1:1 postscale
	T2CONbits.T2CKPS = 0;	// 1:1 prescale
	TMR2 = 0;

	// Set up CCP
	CCP1CONbits.CCP1M = 0b1100; // P1A, P1B active high
	CCP1CONbits.P1M = 0b10; // Half bridge with P1A and P1B as outputs
	PWM1CON = 8; // Deadband in units of 0.25 uS (1 / 16 MHz / 4)

	T2CONbits.TMR2ON = 1;
}

void pwm_deinit(void){
	PIC_BUCK_HI = PIC_BUCK_LO = 0;
	CCP1CONbits.CCP1M = 0; // Turn of CCP-module
	PIC_BUCK_HI = PIC_BUCK_LO = 0;
}

void pwm_set(uint16_t level){
	CCPR1L = (level >> 2);
	CCP1CONbits.DC1B = level & 0b11;
}

void ad_init(void){
	// Enable AN0, 1, 2, 3, 5 input buffer
	ANSELA = 0b00101111;
	ANSELB = 0;
	ANSELC = 0;
	ADCON1bits.PVCFG = 0b10;	// Positive voltage from FVR
	ADCON1bits.NVCFG = 0;		// Negative voltage from Vss
	ADCON2   = 0b10101110;		// Right justify, 20TAD, FOSC/64

	// Voltage reference
	VREFCON0 = 0b10100000;  // FVR1EN, 2.048V
  	ADCON0bits.ADON = 1;  // ADC on
}

uint16_t ad_read(char channel){
 	ADCON0 = (channel<<2) | 0b11;
	while (ADCON0&0b10);
	return ((ADRESH<<8) + ADRESL);
}
