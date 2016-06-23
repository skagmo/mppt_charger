#include "uart.h"

#include <xc.h>
#include <stdint.h>
#include "platform.h"

#define BUFFER_SIZE 32
#define BUFFER_MASK (BUFFER_SIZE - 1)

volatile char rxbuffer[BUFFER_SIZE];
volatile char txbuffer[BUFFER_SIZE];
volatile uint8_t txip, txop, rxip, rxop;

void interrupt isr(void){
	UART_ISR()
}

void uart_init(unsigned long baud){
	unsigned long spbrg_calc = ((_XTAL_FREQ/baud)/16)-1;
	SPBRG = spbrg_calc;
	txip = txop = rxip = rxop = 0;
	TXSTAbits.BRGH = 1;
	RCSTAbits.CREN = 1;
	PIE1bits.RCIE = 1;
	TXSTAbits.TXEN = 1;
	RCSTAbits.SPEN = 1;

	PIR1bits.TXIF = 0;
	PIR1bits.RCIF = 0;
}

char uart_inwaiting(void){
	if (RCSTAbits.OERR){
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
		return 0;
	}
	return (rxip!=rxop);
}

char uart_get(void){
	char c;
	INTCONbits.GIE = 0;
	c = rxbuffer[rxop];
	rxop = (rxop+1) & BUFFER_MASK;
	INTCONbits.GIE = 1;
	return c;
}

void uart_put(char c){
	INTCONbits.GIE = 0;
	if(txop == ((txip+1) & BUFFER_MASK)){
		PIE1bits.TXIE = 1;
		INTCONbits.GIE = 1;
		while(PIE1bits.TXIE);
		INTCONbits.GIE = 0;
	}
	txbuffer[txip] = c;
	txip = (txip+1) & BUFFER_MASK;
	PIE1bits.TXIE = 1;
	INTCONbits.GIE = 1;
}

void uart_puts(const char *s){
	while(*s) uart_put(*s++);
}

void uart_put_block(char c){
	TXREG = c;
	NOP();
	while (PIR1bits.TXIF == 0);
}

void uart_puts_block(const char *s){
	while(*s) uart_put_block(*s++);
}

void uart_put_int(int32_t n){
	uint8_t str[12]; // Sign, 10 digits, termination
	uint8_t len = 0;
	uint8_t j;

	if (n<0){
		uart_put('-');
		n *= -1;
	}
	do{
		str[len++] = (n % 10) + '0';
		n /= 10;
	}while(n);

	for (j=0; j<len; j++) uart_put(str[len-1-j]);
}