#ifndef CONFIG_H
#define	CONFIG_H

#include <stdint.h>

// MPPT state
#define SHUTDOWN 0
#define INC 1
#define DEC 2
#define CV  3

// MPPT related
#define CURR_SENS_VOLT_TRESHOLD	2500	// The current sense monitor is not reliable when the voltage is below 2.5 V
#define DUTY_CYCLE_MAX			450		// Upper limit needed to provide sufficient boot-cap charge
#define DUTY_CYCLE_MIN			200

// Tasks
#define TASK_MPPT 0
#define TASK_LCD 1
#define TASK_PRINT 2
#define TASK_AD 3

typedef struct{
	// Config version and checksum. Only used for saving/loading.
    uint16_t checksum;
	uint16_t config_version;
	
	uint16_t pv_v_min;     // Min. voltage to start charge, must be higher than
	                       // boost-effect from battery with highest duty cycle
	uint16_t bat_i_in_min; // Min. current to keep charging
	uint16_t bat_v_max;    // Max. battery voltage
	uint16_t aux1_shdn;    // Shutdown-voltage for AUX 1
	uint16_t aux2_shdn;    // Shutdown-voltage for AUX 2
	uint16_t shdn_hyst;    // AUX will power on when reaching shdn+hyst volts
	uint8_t aux1_dstate; // Desired state for AUX 1 (overridden by shutdown)
	uint8_t aux2_dstate; // Desired state for AUX 2 (overridden by shutdown)
	uint16_t print_int;
}config_t;

extern config_t config;
extern const char* STATE_STRING[];
extern uint8_t mppt_state;
extern uint16_t duty_cycle;
extern uint16_t pv_v, pv_i, bat_v, bat_i_in, bat_i_out, bat_v_old;

void config_default_load(void);

#endif
