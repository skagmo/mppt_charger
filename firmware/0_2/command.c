//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "command.h"

#include <stdint.h>
#include <string.h>
#include "command_config.h"

char* cmd_toupper_string(char *c);
uint8_t cmd_atoi(char* c, int32_t* n);
uint8_t cmd_atou(char* c, uint32_t* n);
uint8_t cmd_utoa_put(void (*put)(uint8_t), uint32_t n);
uint8_t cmd_itoa_put(void (*put)(uint8_t), int32_t n);
uint8_t cmd_strcpy_put(void (*put)(uint8_t), char *src);
uint8_t cmd_strcpy_put_const(void (*put)(uint8_t), const char *src);

#define CMD_BUF_SIZE 128
uint8_t cmd_buf[CMD_BUF_SIZE];
uint16_t cmd_buf_len;

#ifdef CMD_TNC2_EN
#include "tnc2.h"	// Type TNC2_CALL
#endif

// Scans the command list for a case-insensitive name and returns the index in command list
uint8_t cmd_scan(char* s){
	uint8_t index = 0;
	uint8_t len;

	len = cmd_toupper_string(s) - s;
	while (cmd_list[index].name[0]){	// As long as the setting has a name
		if (!strncmp(s, cmd_list[index].name, len) && (len == strlen(cmd_list[index].name))) return index;
		index++;
	}
	return CMD_SCAN_INVALID;
}

// Displays help for the given command index
void cmd_help_arg(uint8_t ci){
	// Print argument type
	cmd_strcpy_put_const(CMD_PUT, cmd_types[cmd_list[ci].type]);
	cmd_strcpy_put_const(CMD_PUT, " (");

	// Print range or valid parameters
	if (cmd_list[ci].desc){
		switch (cmd_list[ci].type){
			case TYPE_UNDEF:
				break;
			case TYPE_BOOL:
				cmd_strcpy_put_const(CMD_PUT, "0-1");
				break;
			case TYPE_UINT32:
				{
					desc_uint32_t *desc = (desc_uint32_t*)cmd_list[ci].desc;
					cmd_utoa_put(CMD_PUT, desc->min);
					CMD_PUT('-');
					cmd_utoa_put(CMD_PUT, desc->max);
				}
				break;
			case TYPE_UINT32_LIST:
				{
					const uint32_t *list = (const uint32_t*)cmd_list[ci].desc;	//OK with list+i?
					uint8_t i = 0;
					while(i<100){ // i<100?
						cmd_utoa_put(CMD_PUT, *(list+i));
						i++;
						if ( *(list+i) != END_UINT32_LIST) cmd_strcpy_put_const(CMD_PUT, ", ");
						else break;
					}
				}
				break;
			case TYPE_STRING:
				break;
			case TYPE_ENUM:
				{
					const char* desc = (const char*)cmd_list[ci].desc;
					uint8_t i=0;
					while(i<100){ // i<100?
						cmd_strcpy_put_const(CMD_PUT, desc+(i*ENUM_SIZE));
						i++;
						if (*(desc+(i*ENUM_SIZE))) cmd_strcpy_put_const(CMD_PUT, ", ");
						else break;
					}
				}
				break;
			case TYPE_HELP:
				break;
#ifdef CMD_TNC2_EN
			case TYPE_TNC2_CALL:
				CMD_PUT('0' + *(uint8_t*)cmd_list[ci].desc);
				break;
#endif
		}
	}
	CMD_PUT(')');
}

void cmd_help_list(void){
	uint8_t i = 0;
	while (1){
		cmd_strcpy_put_const(CMD_PUT, cmd_list[i].name);
		i++;
		if (cmd_list[i].name[0]) cmd_strcpy_put_const(CMD_PUT, ", ");
		else break;
	}
}

void cmd(char* cmd){
	uint8_t ci = cmd_scan(cmd);		// Find index of command in command list
	char* arg = strchr(cmd, ' ');	// Find pointer to occurance of space
	uint8_t ret;					// Command status return

	if (ci == CMD_SCAN_INVALID) ret = RET_CMD_INVALID;
	else{
		ret = RET_ARG_INVALID;		// Assumes invalid argument at this point
		switch(cmd_list[ci].type){
			case TYPE_BOOL:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					// If the argument has a length of 1 and its value is 0-1
					if ( (*(arg+2) == 0) && (*(arg+1) >= '0') && (*(arg+1) <= '1') ){
						*(uint8_t*)cmd_list[ci].value = *(arg+1) - '0';
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					CMD_PUT('0' + *(uint8_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_INT8:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					int32_t n;
					cmd_atoi(arg+1, &n);
					desc_int8_t *dp = (desc_int8_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(int8_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_itoa_put(CMD_PUT, *(int8_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_UINT8:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					uint32_t n;
					cmd_atou(arg+1, &n);
					desc_uint8_t *dp = (desc_uint8_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(uint8_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_utoa_put(CMD_PUT, *(uint8_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_INT16:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					int32_t n;
					cmd_atoi(arg+1, &n);
					desc_int16_t *dp = (desc_int16_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(int16_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_itoa_put(CMD_PUT, *(int16_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_UINT16:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					uint32_t n;
					cmd_atou(arg+1, &n);
					desc_uint16_t *dp = (desc_uint16_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(uint16_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_utoa_put(CMD_PUT, *(uint16_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_INT32:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					int32_t n;
					cmd_atoi(arg+1, &n);
					desc_int32_t *dp = (desc_int32_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(int32_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_itoa_put(CMD_PUT, *(int32_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_UINT32:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					uint32_t n;
					cmd_atou(arg+1, &n);
					desc_uint32_t *dp = (desc_uint32_t*)cmd_list[ci].desc;
					if ((n >= dp->min) && (n <= dp->max)){
						*(uint32_t*)cmd_list[ci].value = n;
						ret = RET_OK;
						if (cmd_list[ci].set_func) (*(cmd_list[ci].set_func))();
					}
				}
				else if (!arg){
					cmd_utoa_put(CMD_PUT, *(uint32_t*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_UINT32_LIST:
				break;
#ifdef CMD_TNC2_EN
			case TYPE_TNC2_CALL:
				if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
					if (tnc2_encode_call(arg+1, (char*)cmd_list[ci].value, *(uint8_t*)cmd_list[ci].desc)) ret = RET_OK;
				}
				else if (!arg){
					uint8_t j = 0;
					char* s = (char*)cmd_list[ci].value;
					while(1){
						tnc2_decode_call(CMD_PUT, &s[j]);
						j += 7;
						if (s[j]) CMD_PUT(',');
						else break;
					}
					ret = RET_REPLY;
				}
				break;
#endif
			case TYPE_ENUM:
				{
					const char* desc = (const char*)cmd_list[ci].desc;
					if (arg && cmd_list[ci].desc){	// If the command has a space (non-null) after the command (before argument)
						uint8_t len = cmd_toupper_string(arg+1) - arg - 1;
						uint8_t index = 0;

						while (desc[index*ENUM_SIZE]){	// As long as the enum has a name
							if (!strncmp(arg+1, desc+(index*ENUM_SIZE), len) && (len == strlen(desc+(index*ENUM_SIZE)))){
								*(uint8_t*)cmd_list[ci].value = index;
								(*(cmd_list[ci].set_func))();
								ret = RET_OK;
								break;
							}
							index++;
						}
					}
					else if (!arg){
						cmd_strcpy_put_const(CMD_PUT, desc + ((*(uint8_t*)cmd_list[ci].value) * ENUM_SIZE));
						ret = RET_REPLY;
					}
				}
				break;
			case TYPE_HELP:
				if (arg){
					ci = cmd_scan(arg+1);	// Find index of command in command list
					if (ci != CMD_SCAN_INVALID){
						cmd_help_arg(ci);
						ret = RET_REPLY;
					}
				}
				else if (!arg){
					cmd_help_list();
					ret = RET_REPLY;
				}
				break;
			case TYPE_STRING:
				if (arg && cmd_list[ci].desc){
					if (cmd_list[ci].desc){ // If not read-only
						strncpy((char*)cmd_list[ci].value, arg+1, *(uint8_t*)cmd_list[ci].desc);
						ret = RET_REPLY;
					}
				}
				else if (!arg){
					cmd_strcpy_put(CMD_PUT, (char*)cmd_list[ci].value);
					ret = RET_REPLY;
				}
				break;
			case TYPE_UNDEF:
				if (!arg){
					cmd_list[ci].set_func();
					ret = RET_OK;
				}
#ifdef CMD_EXIT_EN
			case TYPE_EXIT:
				if (!arg){
					term_exit_cmd();
					cmd_buf_len = 0;
					ret = RET_EXIT;
				}
				break;
#endif
		}
	}

	// Output general replies
	switch (ret){
		case RET_CMD_INVALID:
			cmd_strcpy_put_const(CMD_PUT, "CMD_INV");
			cmd_strcpy_put_const(CMD_PUT, CMD_END);
			if (CMD_ECHO) CMD_PUT('>');
			break;
		case RET_ARG_INVALID:
			cmd_strcpy_put_const(CMD_PUT, "ARG_INV");
			cmd_strcpy_put_const(CMD_PUT, CMD_END);
			if (CMD_ECHO) CMD_PUT('>');
			break;
		case RET_OK:
			cmd_strcpy_put_const(CMD_PUT, "OK");
			cmd_strcpy_put_const(CMD_PUT, CMD_END);
			if (CMD_ECHO) CMD_PUT('>');
			break;
		case RET_REPLY:
			cmd_strcpy_put_const(CMD_PUT, CMD_END);
			if (CMD_ECHO) CMD_PUT('>');
			break;
		case RET_EXIT:
			cmd_strcpy_put_const(CMD_PUT, "OK");
			cmd_strcpy_put_const(CMD_PUT, CMD_END);
			break;
	}
}

void cmd_init(void){
	if (CMD_ECHO){
		cmd_strcpy_put_const(CMD_PUT, CMD_END);
		CMD_PUT('>');
	}
	cmd_buf_len = 0;
}

// Provides a terminal-like interface
void cmd_byte(char c){
	static char prev;
	switch (c){
		case 0x0D: // CR
		case 0x0A:
			if (prev != 0x0D){	// Only check for LF, as LFCR is unusual
				cmd_buf[cmd_buf_len] = 0;
				if (CMD_ECHO) cmd_strcpy_put_const(CMD_PUT, CMD_END);
				cmd(&cmd_buf[0]);
				cmd_buf_len = 0;
			}

			break;
		case 0x7f:	// PuTTY backspace
			if (cmd_buf_len){
				cmd_buf_len--;
				if (CMD_ECHO) CMD_PUT(0x7f);
			}
			break;
		case 0x08:	// Backspace
			if (cmd_buf_len){
				cmd_buf_len--;
				if (CMD_ECHO) CMD_PUT(0x08);
			}
			break;
		default:
			if (cmd_buf_len < (CMD_BUF_SIZE-1)){
				cmd_buf[cmd_buf_len++] = c;
				if (CMD_ECHO) CMD_PUT(c);
			}													
			break;
	}
	prev = c;
	return;
}

/*** Helpers ***/

// Converts C-string to uppercase until zero or space is found.
// Returns pointer to the zero or space.
char* cmd_toupper_string(char *c){
	while (*c && (*c != ' ')){
		if ((*c >= 97) && (*c <= 122)) *c -= 32;
		c++;
	}
	return c;
}

// Extract integer and check for valid digit until end of string,
// then check if within describer limits (and update).
// Returns true on positive update.
uint8_t cmd_atoi(char* c, int32_t* n){
    *n = 0;
	uint8_t neg_flag = 0;
	if (*c == '-'){
		c++;	// Skip the minus
		neg_flag = 1;
	}
	while(*c){
		if ( (*c < '0') || (*c > '9') ) return 0;
        *n *= 10;
        *n += *c - '0';
		c++;
	}
	if (neg_flag) *n *= -1;
	return 1;
}

uint8_t cmd_atou(char* c, uint32_t* n){
    *n = 0;
	while(*c){
		if ( (*c < '0') || (*c > '9') ) return 0;
        *n *= 10;
        *n += *c - '0';
		c++;
	}
	return 1;
}

// Destination is a put-function (rather than a character array pointer)
uint8_t cmd_utoa_put(void (*put)(uint8_t), uint32_t n){
	char str[11]; // 10 digits plus termination
	uint8_t len = 0;
	uint8_t j;

	do{
		str[len++] = (n % 10) + '0';
		n /= 10;
	}while(n);
	for (j=0; j<len; j++) put(str[len-1-j]);
	return len;
}

// Destination is a put-function (rather than a character array pointer)
uint8_t cmd_itoa_put(void (*put)(uint8_t), int32_t n){
	char str[12]; // Sign, 10 digits, termination
	uint8_t len = 0;
	uint8_t j;

	if (n<0){
		put('-');
		n *= -1;
	}
	do{
		str[len++] = (n % 10) + '0';
		n /= 10;
	}while(n);
	for (j=0; j<len; j++) put(str[len-1-j]);
	return len;
}

// Destination is a put-function (rather than a character array pointer)
uint8_t cmd_strcpy_put(void (*put)(uint8_t), char *src){
    int i;
	for (i=0; src[i]; i++) put(src[i]);
    return i;
}

uint8_t cmd_strcpy_put_const(void (*put)(uint8_t), const char *src){
    int i;
	for (i=0; src[i]; i++) put(src[i]);
    return i;
}
