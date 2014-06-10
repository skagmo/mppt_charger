#ifndef HELPERS_H
#define	HELPERS_H

#include <stdint.h>

void adInit();
void timerInit();
int read_ad(char channel);
unsigned int crc_1021(unsigned int old_crc, char data);
unsigned int crc_calc(char* data, char data_len);

#endif
