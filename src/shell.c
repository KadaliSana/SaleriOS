#include <stddef.h>
#include <stdint.h>

void shell(){  
    while (1) {
        char c = uart_getc();

        if (c == 3){
            uart_puts("SIGINT");
            break;
        }

        else if (c == '\r') {
            uart_puts("\r\n"); // New line
            input[pos] = '\0'; // Null-terminate
            pos = 0; // Reset position
            input[0] = '\0'; // Clear buffer
        }
        else if (c == '\b' || c == 0x7F) { // Handle both \b and DEL (0x7F)
            if (pos > 0) {
                pos--; // Remove last character
                input[pos] = '\0'; // Null-terminate
                // Clear line and reprint buffer
                uart_puts("\r"); // Move to start
                uart_puts(input); // Print current buffer
                uart_puts("  "); // Overwrite with spaces to clear extra chars
                uart_puts("\r"); // Move back to start
                uart_puts(input); // Reprint buffer
            }
        }
        else {
            if (pos < sizeof(input) - 1) {
                input[pos++] = c; // Add character
                uart_putc(c); 
            }
        }
    }
}   