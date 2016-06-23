//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "scheduler.h"

#include <xc.h>
#include <stdint.h>
#include "platform.h"	// SYSCLK define

#define SCH_RAW_TIMER			TMR0
#define SCH_RAW_TIMER_MS_SCALE	125
#define SCH_RAW_TIMER_VAR		uint16_t

sch_task_t sch_tasks[SCH_TASKS_SIZE];
uint32_t sch_timer;			// "Downscaled" timer
SCH_RAW_TIMER_VAR raw_prev;

// Add new task
uint8_t sch_task_init(uint8_t index, uint32_t interval, void (*task)(void)){
	if (index < SCH_TASKS_SIZE){
		sch_tasks[index].interval = interval;
		sch_tasks[index].next = sch_timer + sch_tasks[index].interval;
		sch_tasks[index].task = task;
		return 1;
	}
	return 0;
}

uint8_t sch_task_enable(uint8_t index){
	if (index < SCH_TASKS_SIZE){
		sch_tasks[index].enable = 1;
		sch_tasks[index].next = sch_timer + sch_tasks[index].interval;
		return 1;
	}
	return 0;
}

// Set task inactive / free one slot
uint8_t sch_task_disable(uint8_t index){
	if (index < SCH_TASKS_SIZE){
		sch_tasks[index].enable = 0;
		return 1;
	}
	return 0;
}

// Must be ticked faster than twice the raw timer period
void sch_tick(void){
	uint32_t delta_ms = (SCH_RAW_TIMER - raw_prev)/SCH_RAW_TIMER_MS_SCALE;

	// If step is larger than a millisecond
	if (delta_ms){
		raw_prev += (delta_ms*SCH_RAW_TIMER_MS_SCALE);
		sch_timer += delta_ms;

		// Find the due task(s)
		uint8_t j;
		for (j=0; j<SCH_TASKS_SIZE; j++){
			// Check if task is enabled and due, run it, find next interval
			if ( sch_tasks[j].enable && ((sch_timer-sch_tasks[j].next) < 0x7FFFFFFF) ){
				sch_tasks[j].task();
				sch_tasks[j].next += sch_tasks[j].interval;
			}
		}
	}
}
