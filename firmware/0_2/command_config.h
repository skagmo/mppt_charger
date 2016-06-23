//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

#include "config.h"
#include "platform.h"
#include "tasks.h"

///**** All system-specific describers and settings list - remember to end lists! ****/
const desc_uint16_t shdn_describer = {0, 65000};
const uint8_t uint8_1 = 1;

const cmd_t cmd_list[] = {
{"AUX1-EN",		TYPE_BOOL,		&set_outputs,	&config.aux1_dstate,	&uint8_1},
{"AUX2-EN",		TYPE_BOOL,		&set_outputs,	&config.aux2_dstate,	&uint8_1},
{"AUX1-V-SD",	TYPE_UINT16,	0,				&config.aux1_shdn,		&shdn_describer},
{"AUX2-V-SD",	TYPE_UINT16,	0,				&config.aux2_shdn,		&shdn_describer},
{"BAT-V-MAX",	TYPE_UINT16,	0,				&config.bat_v_max,		&shdn_describer},
{"CHG-I-MIN",	TYPE_UINT16,	0,				&config.bat_i_in_min,	&shdn_describer},
{"PRINT-INT",	TYPE_UINT16,	&set_print_int,	&config.print_int,		&shdn_describer},
{"PV-V-MIN",	TYPE_UINT16,	0,				&config.pv_v_min,		&shdn_describer},
{"HELP",		TYPE_HELP,		0,				0,						0},
{"SD-HYST",		TYPE_UINT16,	0,				&config.shdn_hyst,		&shdn_describer},
{"",0,0,0,0}
};

#endif

