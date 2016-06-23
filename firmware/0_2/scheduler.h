//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef struct{
	uint8_t enable;
	uint32_t interval;	 // Can maximum be half of the variable size
	uint32_t next;
	void (*task)(void);
}sch_task_t;

#define SCH_TASKS_SIZE 4
extern sch_task_t sch_tasks[SCH_TASKS_SIZE];

uint8_t sch_task_init(uint8_t index, uint32_t interval, void (*task)(void));
uint8_t sch_task_enable(uint8_t index);
uint8_t sch_task_disable(uint8_t index);
void sch_tick(void);

#endif
