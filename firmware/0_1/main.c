#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"
#include "uart.h"
#include "perf.h"

// MPPT related
#define PV_V_MIN				13000	// Min. voltage to start charge
#define BAT_I_IN_MIN			40		// Min. current to keep charging
#define BAT_V_MAX				12500	// Max. battery voltage
#define CURR_SENS_VOLT_TRESHOLD	2500	// The current sense monitor is not reliable when the voltage is below 2.5 V
#define DUTY_CYCLE_MAX			120
#define DUTY_CYCLE_MIN			40

// MPPT states
#define SHUTDOWN			0
#define DECREASING			1
#define INCREASING			2

int main(int argc, char** argv) {
	// Initialize peripherals etc.
	general_init();
	pwm_deinit();
	ad_init();
	uart_init(38400);
	__delay_ms(20);

	// Enable interrupts
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

	uint8_t mppt_state = SHUTDOWN;
	uint8_t duty_cycle = DUTY_CYCLE_MAX;
	uint16_t pv_v, pv_i, bat_v, bat_i_in, bat_i_out, bat_v_old;

	// Main while-loop
	while(1){

		// Manually control duty-cycle and standby-mode with UART-commands
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

		// Read analog values
		pv_v = (uint32_t)ad_read(AD_PV_V) * 21790 / 1000;
		pv_i = (uint32_t)ad_read(AD_PV_I) * 2;
		bat_v = (uint32_t)ad_read(AD_BAT_V) * 21904 / 1000;
		bat_i_in = (uint32_t)ad_read(AD_BAT_IN_I) * 2;
		bat_i_out = (uint32_t)ad_read(AD_BAT_OUT_I) * 2;

		// Run the MPPT-algorithm
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

		// Print current, voltage and other states on the UART in an NMEA-like format
		uart_puts("$");
		uart_put_int(pv_v);
		uart_puts(",");
		if (pv_v > CURR_SENS_VOLT_TRESHOLD) uart_put_int(pv_i);
		else uart_puts(",");
		uart_puts(",");
		uart_put_int(bat_v);
		uart_puts(",");
		if (bat_v > CURR_SENS_VOLT_TRESHOLD) uart_put_int(bat_i_in);
		uart_puts(",");
		if (bat_v > CURR_SENS_VOLT_TRESHOLD) uart_put_int(bat_i_out);
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
		// if (mppt_state == SHUTDOWN) OSCCONbits.IDLEN = 0;
		// else OSCCONbits.IDLEN = 1;
		// WDTCONbits.SWDTEN = 1;
		// SLEEP();
		// WDTCONbits.SWDTEN = 0;
		// If the device is in sleep with WDT enabled this will wake the device.
		// WDTPS is set in units of 4 ms. 1:32 -> 128 ms.
		// RC_IDLE MODE ensures clock will run.
		// 15 mA current consumption with while, 13 mA med SLEEP.
		// Saving 2 mA -> not necessary

		// Wait for about 100 mS
		{
			uint16_t j;
			for (j=0; j<100; j++) __delay_ms(1);
		}
	}

	return 0;
}

