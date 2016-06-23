#ifndef PERF_H
#define	PERF_H

#include <stdint.h>

void pwm_init(void);
void pwm_deinit(void);
void pwm_set(uint8_t level);
void ad_init(void);
int ad_read(char channel);

#endif
