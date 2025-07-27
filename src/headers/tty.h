#ifndef _kernel_tty
#define _kernel_tty

#include <stddef.h>

void uart_init(void);
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);

#endif