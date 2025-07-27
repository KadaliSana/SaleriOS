#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <tty.h>

// The base address for all peripheral registers on the Raspberry Pi 4.
static const uint32_t MMIO_BASE = 0xFE000000;

// Helper function to write to a memory-mapped I/O register.
static inline void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)(MMIO_BASE + reg) = data;
}

// Helper function to read from a memory-mapped I/O register.
static inline uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t*)(MMIO_BASE + reg);
}

// A simple delay loop using assembly.
static inline void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : "=r"(count) : [count]"0"(count) : "cc");
}

// Register offsets from MMIO_BASE.
enum {
    // GPIO register offsets.
    GPIO_BASE   = 0x00200000,
    GPPUD       = (GPIO_BASE + 0x94),
    GPPUDCLK0   = (GPIO_BASE + 0x98),

    // UART0 register offsets.
    UART0_BASE  = 0x00201000,
    UART0_DR    = (UART0_BASE + 0x00),
    UART0_FR    = (UART0_BASE + 0x18),
    UART0_IBRD  = (UART0_BASE + 0x24),
    UART0_FBRD  = (UART0_BASE + 0x28),
    UART0_LCRH  = (UART0_BASE + 0x2C),
    UART0_CR    = (UART0_BASE + 0x30),
    UART0_ICR   = (UART0_BASE + 0x44),

    // Mailbox register offsets.
    MBOX_BASE   = 0x0000B880,
    MBOX_READ   = (MBOX_BASE + 0x00),
    MBOX_STATUS = (MBOX_BASE + 0x18),
    MBOX_WRITE  = (MBOX_BASE + 0x20)
};

// Mailbox message buffer to set the UART clock rate.
// It must be 16-byte aligned.
volatile unsigned int __attribute__((aligned(16))) mbox[9] = {
    9 * 4, 0, 0x38002, 12, 8, 2, 3000000, 0, 0
};

void uart_init() {
    // 1. Disable UART0.
    mmio_write(UART0_CR, 0x00000000);

    // 2. Set up GPIO pins 14 and 15 for alternative function (UART).
    mmio_write(GPPUD, 0);
    delay(150);
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    mmio_write(GPPUDCLK0, 0x00000000);

    // 3. Clear any pending UART interrupts.
    mmio_write(UART0_ICR, 0x7FF);

    // 4. Use the mailbox to set the UART clock to 3MHz.
    unsigned int r = (((unsigned int)(uintptr_t)&mbox) & ~0xF) | 8;
    while (mmio_read(MBOX_STATUS) & 0x80000000) {}
    mmio_write(MBOX_WRITE, r);
    while ((mmio_read(MBOX_STATUS) & 0x40000000) || mmio_read(MBOX_READ) != r) {}

    // 5. Set the baud rate to 115200.
    // Baud rate divisor = 3,000,000 / (16 * 115200) = 1.627
    mmio_write(UART0_IBRD, 1);  // Integer part of divisor.
    mmio_write(UART0_FBRD, 40); // Fractional part of divisor.

    // 6. Enable FIFOs and set data to 8-bit, 1-stop bit, no parity.
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // 7. Re-enable UART0, along with transmit and receive.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

// Sends a single character over UART.
void uart_putc(unsigned char c) {
    // Wait until the transmit FIFO is no longer full (FR bit 5 is 0).
    while (mmio_read(UART0_FR) & (1 << 5)) {}
    mmio_write(UART0_DR, c);
}

// Receives a single character from UART.
unsigned char uart_getc() {
    // Wait until the receive FIFO is no longer empty (FR bit 4 is 0).
    while (mmio_read(UART0_FR) & (1 << 4)) {}
    return (unsigned char)mmio_read(UART0_DR);
}

// Sends a null-terminated string over UART.
void uart_puts(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        uart_putc((unsigned char)str[i]);
    }
}
