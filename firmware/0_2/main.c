#include <xc.h>
#include <stdint.h>

#include "platform.h"
#include "uart.h"
#include "perf.h"
#include "lcd.h"
#include "scheduler.h"
#include "command.h"
#include "config.h"
#include "tasks.h"

int main(int argc, char** argv) {
	// Initialize peripherals etc.
	general_init();
	pwm_deinit();
	ad_init();
	uart_init(38400);
	spi_init();
	lcd_init();
	t0_init();  // Used for scheduler
	config_default_load();
	AUX1_EN = config.aux1_dstate;
	AUX2_EN = config.aux2_dstate;
	__delay_ms(5);
	
	// Enable interrupts
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
	
	// Show splash screen
	LCD_BL = 1;
	lcd_clear();
	lcd_bitmap(se_logo, 127, 6);
	lcd_cursor(25, 7);
	lcd_puts("MPPT charger");
	lcd_refresh();
	delay_ms(2000);

	// Initialize tasks
	sch_task_init(TASK_LCD, 1000, &lcd_task); sch_task_enable(TASK_LCD);
	sch_task_init(TASK_MPPT, 100, &mppt_task); sch_task_enable(TASK_MPPT);
	sch_task_init(TASK_AD, 100, &ad_task); sch_task_enable(TASK_AD);
	set_print_int();
	
	// Main while-loop
	while(1){
		// Manually control duty-cycle and standby-mode with UART-commands
		while (uart_inwaiting()) cmd_byte(uart_get());

		sch_tick();
		
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
	}
	
	return (0);
}
