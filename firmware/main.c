#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "settings.h"
#include "platform.h"
#include "uart.h"

#define AD_PV_I			5
#define AD_BAT_IN_I		6
#define AD_PV_V			7
#define AD_BAT_V		8
#define AD_BAT_OUT_I	9

#define PIC_BUCK_HI			LATCbits.LATC5
#define PIC_BUCK_LO			LATCbits.LATC4
#define MIN_VOLT_TRESHOLD	2500
#define DUTY_CYCLE_MAX		120
#define DUTY_CYCLE_MIN		40
//PWM fra 0-max -> 8-123

// MPPT related
#define PV_V_MIN			13000	// Min. voltage to start charge
#define BAT_I_IN_MIN		40		// Min. current to keep charging
#define BAT_V_MAX			12500	// Max. battery voltage

// MPPT states
#define SHUTDOWN			0
#define DECREASING			1
#define INCREASING			2

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

int main(int argc, char** argv) {
	general_init();
	pwm_deinit();
	ad_init();
	uart_init(38400);
	__delay_ms(20);

	uint8_t mppt_state = SHUTDOWN;
	uint8_t duty_cycle;
	uint16_t pv_v, pv_i, bat_v, bat_i_in, bat_i_out, bat_v_old;

	// PV_V BIAS = +10
	// BAT_I_OUT_BIAS = +14
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

	while(1){
		while (uart_inwaiting()){
			switch (uart_get()){
				case 'u':
					if (duty_cycle<DUTY_CYCLE_MAX){
						duty_cycle++;
						pwm_set(duty_cycle);
					}
					break;
				case 'd':
					if (duty_cycle>DUTY_CYCLE_MIN){
						duty_cycle--;
						pwm_set(duty_cycle);
					}
					break;
				case 'o':
					pwm_deinit();
					break;
				case 'p':
					pwm_init();
					break;

			}
		}

		pv_v = (uint32_t)ad_read(AD_PV_V) * 21790 / 1000;
		pv_i = (uint32_t)ad_read(AD_PV_I) * 2;
		bat_v = (uint32_t)ad_read(AD_BAT_V) * 21904 / 1000;
		bat_i_in = (uint32_t)ad_read(AD_BAT_IN_I) * 2;
		bat_i_out = (uint32_t)ad_read(AD_BAT_OUT_I) * 2;
		
		switch (mppt_state){
			case SHUTDOWN:
				// If solar panel open voltage is above treshold and battery can take charge
				if ( (pv_v > PV_V_MIN) &&
				     (bat_v < BAT_V_MAX) ){
					duty_cycle = DUTY_CYCLE_MAX;	// Start with max duty cycle - prevents reverse current
					pwm_set(duty_cycle);
					pwm_init();
					mppt_state = DECREASING;
				}
				break;

			case DECREASING:
				// If battery is not being charged or is full
				if ((bat_i_in < BAT_I_IN_MIN) || (bat_v >= BAT_V_MAX)){
					mppt_state = SHUTDOWN;
					pwm_deinit();
					break;
				}
				// If the battery voltage increases with the last adjustment, continue in this direction
				if ((bat_v >= bat_v_old) && (duty_cycle > DUTY_CYCLE_MIN)) duty_cycle--;
				else{
					duty_cycle++;
					mppt_state = INCREASING;
				}
				pwm_set(duty_cycle);
				break;

			case INCREASING:
				// If battery is not being charged or is full
				if ((bat_i_in < BAT_I_IN_MIN) || (bat_v >= BAT_V_MAX)){
					mppt_state = SHUTDOWN;
					pwm_deinit();
					break;
				}
				// If the battery voltage increases with the last adjustment, continue in this direction
				if ((bat_v >= bat_v_old) && (duty_cycle < DUTY_CYCLE_MAX)) duty_cycle++;
				else{
					duty_cycle--;
					mppt_state = DECREASING;
				}
				pwm_set(duty_cycle);
				break;
		}

		bat_v_old = bat_v;

		uart_puts("$");
		uart_put_int(pv_v);
		uart_puts(",");
		if (pv_v > MIN_VOLT_TRESHOLD) uart_put_int(pv_i);
		else uart_puts(",");
		uart_puts(",");
		uart_put_int(bat_v);
		uart_puts(",");
		if (bat_v > MIN_VOLT_TRESHOLD) uart_put_int(bat_i_in);
		uart_puts(",");
		if (bat_v > MIN_VOLT_TRESHOLD) uart_put_int(bat_i_out);
		uart_puts(",");
		uart_put_int(duty_cycle);
		uart_puts(",");
		switch (mppt_state){
			case SHUTDOWN: uart_puts("SHDN"); break;
			case INCREASING: uart_puts("INC"); break;
			case DECREASING: uart_puts("DEC"); break;
		}
		uart_puts("\r\n");

		//	while(PIE1bits.TXIE);
		//	__delay_ms(10);
		// Hvis enheten er i sleep på WDT timeout vil dette vekke enheten
		// WDTPS satt i config, units of 4 ms. 1:32 -> 128 ms
		// 38 mA til 14 mA i shutdown. Merkelig nok 46 mA i IDLE
		// RC_IDLE MODE holder på klokke
		// 15 mA SHDN med while
		// 13 mA med SLEEP
		//	if (mppt_state == SHUTDOWN) OSCCONbits.IDLEN = 0;
		//	else OSCCONbits.IDLEN = 1;
		//	WDTCONbits.SWDTEN = 1;
		//	SLEEP();
		//	WDTCONbits.SWDTEN = 0;

		uint16_t j;
		for (j=0; j<100; j++) __delay_ms(1);
		
	}

	return 0;
}

