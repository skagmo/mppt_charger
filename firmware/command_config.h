#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

#include "settings.h"

/**** All system-specific describers and settings list - remember to end lists! ****/

const desc_uint32_t freq_describer = {137000000, 174000000};
const uint32_t baud_describer[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, END_UINT32_LIST};
//const char* profile_describer[] = {"TNC2_AFSK", "TNC2_GFSK", END_ENUM};
const desc_uint32_t chg_vmax_desc = {5000, 15000};

const cmd_t cmd_list[] = {
// Name			Type				Set function		Value ptr.						Desc. ptr.
{"FREQUENCY",	TYPE_UINT32,		0,					0,								&freq_describer},
{"LNA_ON",		TYPE_BOOL,			0,					0,								0},
{"TERM_BAUD",	TYPE_UINT32_LIST,	0,					0,								baud_describer},
{"PROFILE",		TYPE_ENUM,			0,					0,								0}, //&profile_describer
{"HELP",		TYPE_HELP,			0,					0,								0},
{"CHG_VMAX",	TYPE_UINT32,		0,					&main_set.chg_vmax,				&chg_vmax_desc},
{"",			0,					0,					0,								0}
};

#endif


/*
HELP, HELP x, CONVERSE
VERSION
PROFILE
SYMRATE
DEVIATION
CHANBW
SYNCMODE
SYNCWORD
PREAMB_LEN
STATE_AFTER_TX
BIAS_AFTER_TX
FREQUENCY
POWER
TERM_PORT
TERM_RATE
TERM_END
GPS_PORT
GPS_RATE
SYS_TEMP
SYS_VOLT
AX25_DEST
AX25_SOURCE
AX25_PATH
MICE_COMMENT
MICE_COMMENT_INT
MICE_MESSAGE
PERTX_INT
GPS_TO_TERM
LNA_ON
FAN_ON
RSSI
*/
