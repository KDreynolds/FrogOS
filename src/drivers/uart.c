#include <stdint.h>
#include <stddef.h>
#include "kernel/uart.h"

// UART registers
#define UART0_BASE 0x09000000
#define UART0_DR   (UART0_BASE + 0x00)
#define UART0_FR   (UART0_BASE + 0x18)
#define UART0_IBRD (UART0_BASE + 0x24)
#define UART0_FBRD (UART0_BASE + 0x28)
#define UART0_LCRH (UART0_BASE + 0x2C)
#define UART0_CR   (UART0_BASE + 0x30)
#define UART0_IMSC (UART0_BASE + 0x38)

void uart_init() {
    // Disable UART0
    *((volatile uint32_t*)(UART0_CR)) = 0x00000000;

    // Setup the baud rate: 115200 baud
    // Assuming 48MHz clock
    *((volatile uint32_t*)(UART0_IBRD)) = 26;
    *((volatile uint32_t*)(UART0_FBRD)) = 3;

    // Enable FIFO & 8 bit data transmission (1 stop bit, no parity)
    *((volatile uint32_t*)(UART0_LCRH)) = 0x70;

    // Mask all interrupts
    *((volatile uint32_t*)(UART0_IMSC)) = 0x7FF;

    // Enable UART0, receive & transfer part of UART
    *((volatile uint32_t*)(UART0_CR)) = 0x301;
}

void uart_putc(unsigned char c) {
    // Wait for UART to become ready to transmit
    while (*((volatile uint32_t*)(UART0_FR)) & (1 << 5));
    *((volatile uint32_t*)(UART0_DR)) = c;
}

unsigned char uart_getc() {
    // Wait for UART to have received something
    while (*((volatile uint32_t*)(UART0_FR)) & (1 << 4));
    return *((volatile uint32_t*)(UART0_DR));
}

void uart_puts(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        uart_putc((unsigned char)str[i]);
    }
}