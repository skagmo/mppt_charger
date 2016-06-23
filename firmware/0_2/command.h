//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef COMMAND_H
#define	COMMAND_H

#include <stdint.h>
#include "uart.h"

#define CMD_ECHO			1
#define CMD_PUT				uart_put
#define CMD_END				"\r\n"

#define ENUM_SIZE			10
#define END_UINT32_LIST		0xffffffff
#define END_ENUM			""
#define CMD_SCAN_INVALID	0xff
#define CMD_LEN_MAX			32

// Valid types are:
#define TYPE_UNDEF			0	// Only caller-function is used
#define TYPE_BOOL			1	// (0/OFF/FALSE, 1/ON/TRUE)
#define TYPE_INT8			2	//
#define TYPE_UINT8			3	// 0-255
#define TYPE_INT16			4	//
#define TYPE_UINT16			5	//
#define TYPE_INT32			6	//
#define TYPE_UINT32			7	//
#define TYPE_UINT32_LIST	8	// [1200, 2400, 4800, ...], end with 0xffffffff
#define TYPE_STRING			9	// " "
#define TYPE_ENUM			10	// ["TNC2_GFSK", ...], end with 0
#define TYPE_HELP			11	//
#define TYPE_TNC2_CALL		12	// Callsign on the form LA3JPA-2,WIDE1-1 etc.
#define TYPE_EXIT			13

static const char* cmd_types[] = {"UNDEF", "BOOL", "INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32", "UINT32_LIST", "STRING", "ENUM", "HELP", "TNC2_CALL"};

// Reply types
#define RET_CMD_INVALID		0
#define RET_ARG_INVALID		1
#define RET_OK				2
#define RET_REPLY			3
#define RET_EXIT			4

typedef struct{
	const char* name;		// Command name
	uint8_t type;			// Command argument type
	void (*set_func)(void);	// Function will be executed after setting
	void *value;			// Value which is changed
	void *desc;				// Describer of valid range / parameters
}cmd_t;

/**** Describers ****/

typedef struct{
	int8_t min;
	int8_t max;
}const desc_int8_t;

typedef struct{
	uint8_t min;
	uint8_t max;
}const desc_uint8_t;

typedef struct{
	int16_t min;
	int16_t max;
}const desc_int16_t;

typedef struct{
	uint16_t min;
	uint16_t max;
}const desc_uint16_t;

typedef struct{
	int32_t min;
	int32_t max;
}const desc_int32_t;

typedef struct{
	uint32_t min;
	uint32_t max;
}const desc_uint32_t;

// UINT32_LIST		const uint32_t values[]
// STRING			const uint8_t max_len;
// TYPE_ENUM		const char* values[]
// TYPE_HELP
// TYPE_BOOL		pointer to uint8_t bit position

void cmd_init(void);
void cmd_byte(char c);

#endif
