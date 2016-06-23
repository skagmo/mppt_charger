#ifndef PERF_H
#define	PERF_H

#include <stdint.h>

void t0_init(void);
void spi_init(void);
void spi(char data);
void pwm_init(void);
void pwm_deinit(void);
void pwm_set(uint16_t level);
void ad_init(void);
uint16_t ad_read(char channel);

#endif
