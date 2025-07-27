#include <stdio.h>

#if defined(__is_libk)
#include <tty.h>
#endif

int putc(char ic) {
#if defined(__is_libk)
	uart_putc(ic);
#else
	// TODO: Implement stdio and the write system call.
#endif
	return (int)ic;
}
