#include <stddef.h>
#include <stdint.h>
#include <tty.h>
#include <shell.h>

#if defined(__cplusplus)
extern "C" // Use C linkage for kmain.
#endif

// The kernel's main entry point.
void kmain(void) {
    uart_init();
    uart_puts("Welcome to SaleriOS!!!\r\n");
    uart_puts(">");
    shell();
}