#ifndef STRING_H
#define STRING_H

#include "stdint.h"

char* str_toupper(char *c);
uint8_t str_atoi(char* c, int32_t* n);
uint8_t str_atou(char* c, uint32_t* n);
uint8_t str_utoa_put(void (*put)(uint8_t), uint32_t n);
uint8_t str_itoa_put(void (*put)(uint8_t), int32_t n);
uint8_t str_cpy_put(void (*put)(uint8_t), char *src);
uint8_t str_cpy_put_const(void (*put)(uint8_t), const char *src);
uint32_t str_convert_uint(char* c, uint8_t len);
uint8_t str_cpy_len(char* dest, char *src);
void str_cpy(char* dest, const char *src);
void str_htoa_put(void (*put)(uint8_t), uint16_t n, uint8_t n_len);

#endif