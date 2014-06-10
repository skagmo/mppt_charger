#ifndef UARTRING_H
#define UARTRING_H

#include <stdint.h>

/* The define below should be used in the ISR! */
#define UART_ISR()                      \
	if(PIR1bits.RCIF){                  \
		rxbuffer[rxip] = RCREG;         \
		rxip = (rxip+1) & BUFFER_MASK;  \
	}                                   \
                                        \
	if(PIR1bits.TXIF && PIE1bits.TXIE){ \
		TXREG = txbuffer[txop];         \
		txop = (txop+1) & BUFFER_MASK;  \
		if(txop == txip) TXIE = 0;      \
	}                                   \

void uart_init(unsigned long baud);
char uart_inwaiting(void);
char uart_get(void);
void uart_put(char c);
void uart_puts(const char *s);

void uart_put_block(char c);
void uart_puts_block(const char *s);

void uart_put_int(int32_t n);

#endif
