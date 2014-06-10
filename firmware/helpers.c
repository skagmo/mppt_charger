#include "helpers.h"

#include <xc.h>
#include <stdint.h>

void adInit(){
	ANSEL    = 0b01110100;
	ANSELH   = 0b00000001;
  	ADCON1   = 0b00001000;  // FVR reference 0b00001000
	ADCON2   = 0b10101110;  // 20TAD, FOSC/64
	VREFCON0 = 0b10010000;  // FVR1EN, 2.048V
  	ADCON0   = 0b00000001;  // ADC on
}

void timerInit(){
	T0CON = 0b10000111;  // Timer on, 16-bit, 1:256 prescaler -> (FOSC/4)/256 = 64 uS step (16 MHz) -> 200mS = 3125 step
}

int read_ad(char channel){
	long result = 0;
	for (char j=0; j<8; j++){
 		ADCON0 = (channel<<2) | 0b11;
		while (ADCON0&0b10);
		result += (ADRESH<<8) + ADRESL;
	}
	return result>>3;
}

unsigned int crc_1021(unsigned int old_crc, char data){
    unsigned int crc;
    unsigned int x;
    x = ((old_crc>>8) ^ data) & 0xFF;
    x ^= x>>4;
    crc = (old_crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    return crc;
}

unsigned int crc_calc(char* data, char data_len){
    unsigned int crc_value = 0;
    for (char j=0; j<data_len; j++) crc_value = crc_1021(crc_value, data[j]);
    return crc_value;
}