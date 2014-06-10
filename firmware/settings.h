#ifndef SETTINGS_H
#define	SETTINGS_H

#include "stdint.h"

typedef struct{
	uint32_t chg_vmax;
}main_set_t;

extern main_set_t main_set;

void set_defaults(main_set_t* s);

#endif