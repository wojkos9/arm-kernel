#include <stdint.h>

#include "printf.h"

#define UART0_BASE 0x9000000

void _putchar(char c) {
    *(volatile uint32_t *)(UART0_BASE) = c;
}

void start() {
    printf("Hi %lx\n", 0x1234);
}


