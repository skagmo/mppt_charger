#include "_string.h"

// Converts C-string to uppercase until zero or space is found.
// Returns pointer to the zero or space.
char* str_toupper(char *c){
	while (*c && (*c != ' ')){
		if ((*c >= 97) && (*c <= 122)) *c -= 32;
		c++;
	}
	return c;
}

// Extract integer and check for valid digit until end of string,
// then check if within describer limits (and update).
// Returns true on positive update.
uint8_t str_atoi(char* c, int32_t* n){
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

uint8_t str_atou(char* c, uint32_t* n){
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
uint8_t str_utoa_put(void (*put)(uint8_t), uint32_t n){
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
uint8_t str_itoa_put(void (*put)(uint8_t), int32_t n){
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
uint8_t str_cpy_put(void (*put)(uint8_t), char *src){
    int i;
	for (i=0; src[i]; i++) put(src[i]);
    return i;
}

uint8_t str_cpy_put_const(void (*put)(uint8_t), const char *src){
    int i;
	for (i=0; src[i]; i++) put(src[i]);
    return i;
}

uint8_t str_cpy_len(char* dest, char *src){
    int i;
	for (i=0; src[i] != 0; i++) dest[i] = src[i];
    return i;
}

void str_cpy(char* dest, const char *src){
    while (*src != 0) *dest++ = *src++;
	*dest++ = 0;	// Added zero terminal
}

void str_htoa_put(void (*put)(uint8_t), uint16_t n, uint8_t n_len){
	uint8_t j;
	uint16_t temp;
	uint16_t bitshift = (n_len-1)*4;
	uint16_t mask = 0xF << bitshift;
	for (j=0; j<n_len; j++){
		temp = (n & mask) >> bitshift;
		n <<= 4;
		if(temp < 10) put(temp + '0');
		else put(temp + 55);
	}
}
