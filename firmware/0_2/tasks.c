#include "tasks.h"

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "scheduler.h"
#include "config.h"
#include "platform.h"
#include "lcd.h"
#include "uart.h"
#include "perf.h"

void set_print_int(void){
	if (config.print_int){
		sch_task_init(TASK_PRINT, config.print_int, &print_task); sch_task_enable(TASK_PRINT);
		sch_task_enable(TASK_PRINT);
	}
	else sch_task_disable(TASK_PRINT);
}

void set_outputs(void){
	if ( bat_v > (config.aux1_shdn+config.shdn_hyst) ) AUX1_EN = config.aux1_dstate;
	if ( bat_v > (config.aux2_shdn+config.shdn_hyst) ) AUX2_EN = config.aux2_dstate;
	
	// Disable outputs if low voltage
	if (AUX1_EN && (bat_v < config.aux1_shdn)) AUX1_EN = 0;
	if (AUX2_EN && (bat_v < config.aux2_shdn)) AUX2_EN = 0;
}

void lcd_task(void){
	uint8_t msg[50];
	uint32_t pv_p = (uint32_t)pv_i*pv_v/1000;
	uint32_t bat_p_in = (uint32_t)bat_i_in*bat_v/1000;
	uint32_t bat_p_out = (uint32_t)bat_i_out*bat_v/1000;

	lcd_clear();
	lcd_cursor(20, 0); lcd_puts("- Measurement -");
	sprintf(msg, "Sta. %s", STATE_STRING[mppt_state]); lcd_cursor(0, 1); lcd_puts(msg);
	sprintf(msg, "PV   %umV %umA", pv_v, pv_i); lcd_cursor(0, 2); lcd_puts(msg);
	sprintf(msg, "     %lumW", pv_p); lcd_cursor(0, 3); lcd_puts(msg);
	sprintf(msg, "Bat. %umV %umA", bat_v, bat_i_in); lcd_cursor(0, 4); lcd_puts(msg); 
	sprintf(msg, "     %lumW", bat_p_in); lcd_cursor(0, 5); lcd_puts(msg);
	sprintf(msg, "Eff. %u%%", bat_p_in*100/pv_p); lcd_cursor(0, 6); lcd_puts(msg);
	sprintf(msg, "Load %umA %lumW", bat_i_out, bat_p_out); lcd_cursor(0, 7); lcd_puts(msg);
	lcd_refresh();
}

void print_task(void){
	uint8_t msg[100];
	
	// Print current, voltage and other states on the UART in an NMEA-like format - snprintf not available in XC8!
	if (pv_v > CURR_SENS_VOLT_TRESHOLD) sprintf(msg, "$%u,%u,%u,%u,%u,%u,%s\r\n",
		pv_v, pv_i, bat_v, bat_i_in, bat_i_out, duty_cycle, STATE_STRING[mppt_state]);
	else sprintf(msg, "$%u,,%u,,,%u,%s\r\n", pv_v, bat_v, duty_cycle, STATE_STRING[mppt_state]);
	uart_puts(msg);
}

void ad_task(void){
	// Read analog values
	pv_v = ad_read(AD_PV_V) * 22;
	bat_v = ad_read(AD_BAT_V) * 22;
	pv_i = ad_read(AD_PV_I) * 2;
	bat_i_in = ad_read(AD_BAT_IN_I) * 2;
	bat_i_out = ad_read(AD_BAT_OUT_I) * 2;
	
	set_outputs();
}

void mppt_task(void){
	// Run the MPPT-algorithm
	switch (mppt_state){
		case SHUTDOWN:
			// If solar panel open voltage is above treshold
			if ((pv_v > config.pv_v_min) && (bat_v < config.bat_v_max)){
				mppt_state = DEC;
				duty_cycle = DUTY_CYCLE_MAX;	// Start with max duty cycle - prevents reverse current
				pwm_set(duty_cycle);
				pwm_init();
			}
			break;

		case INC:
		case DEC:
			// If battery is not being charged -> SHUTDOWN
			if (bat_i_in < config.bat_i_in_min){
				mppt_state = SHUTDOWN;
				sch_tasks[TASK_MPPT].next += 20000;	// Postpone by 20 seconds
				pwm_deinit();
				break;
			}
			// If battery is at maximum voltage -> CV
			if (bat_v >= config.bat_v_max){
				mppt_state = CV;
				duty_cycle--;
				pwm_set(duty_cycle);
				break;
			}

			if (mppt_state == DEC){
				// If the battery voltage decreased or duty cycle has reached limit, go to INCREASE-state
				if ((bat_v < bat_v_old) || (duty_cycle <= DUTY_CYCLE_MIN)){
					mppt_state = INC;
					duty_cycle++;
				}
				else duty_cycle--;
			}
			else{
				// If the battery voltage decreased or duty cycle has reached limit, go to INCREASE-state
				if ((bat_v < bat_v_old) || (duty_cycle >= DUTY_CYCLE_MAX)){
					mppt_state = DEC;
					duty_cycle--;
				}
				else duty_cycle++;
			}
			pwm_set(duty_cycle);
			break;

		case CV:
			// If the battery voltage decreased or duty cycle has reached limit, go to INCREASE-state
			if (bat_v < config.bat_v_max){
				if (duty_cycle < DUTY_CYCLE_MAX) duty_cycle++;
				else{
					mppt_state = DEC;	// Can't increase more -> MPPT
					break;
				}
			}
			else{
				if (duty_cycle > DUTY_CYCLE_MIN) duty_cycle--;
				else{
					mppt_state = SHUTDOWN;
					sch_tasks[TASK_MPPT].next += 20000;	// Postpone by 20 seconds
					pwm_deinit();
					break;
				}
			}
			pwm_set(duty_cycle);
			break;
	}
	bat_v_old = bat_v;
}