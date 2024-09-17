#include <stdint.h>
#include "printf.h"

#define UART0_BASE 0x9000000
#define UART0_INTERRUPT (37u)
#define UART_TXFF 0x20
#define UART_RXFE 0x10

#define CR_UARTEN       (1 << 0u)
#define IMSC_RXIM	(1u << 4u)

typedef volatile struct __attribute__((packed)) {
    uint32_t dr;                /* 0x0 Data Register */
    uint32_t rsrecr;            /* 0x4 Receive status / error clear register */
    uint32_t _reserved0[4];     /* 0x8 - 0x14 reserved */
    const uint32_t fr;          /* 0x18 Flag register */
    uint32_t _reserved1;        /* 0x1C reserved */
    uint32_t ilpr;              /* 0x20 Low-power counter register */
    uint32_t ibrd;              /* 0x24 Integer baudrate register */
    uint32_t fbrd;              /* 0x28 Fractional baudrate register */
    uint32_t lcrh;              /* 0x2C Line control register */
    uint32_t cr;                /* 0x30 Control register */
	uint32_t ifls;				/* 0x34 FIFO level select register */
	uint32_t imsc;				/* 0x38 Interrupt mask set/clear register */
	uint32_t ris;				/* 0x3C Raw interrupt status register */
	uint32_t mis;				/* 0x40 Masked interrupt status register */
	uint32_t isc;				/* 0x44 Interrupt clear register */
} uart_regs;

uart_regs *uart0 = (void*)UART0_BASE;

void writel(unsigned long addr,  uint32_t val) {
    *(volatile uint32_t *)addr = val;
}

uint32_t readl(unsigned long addr) {
    return *(volatile uint32_t *)addr;
}

void putc(char c) {
    while(uart0->fr & UART_TXFF)
        ;
    uart0->dr = c;
}

char try_getc() {
    if (uart0->fr & UART_RXFE)
        return 0;

    return uart0->dr;
}

void _putchar(char c) {
    if (c == '\r')
        putc('\n');
    putc(c);
}

void __attribute__((interrupt)) irq_handler() {
    printf("int\n");
}

void start() {

    printf("hello\n");
    for(;;) {
        char c = try_getc();
        if (c != 0) {
            _putchar(c);
        }
    }
}


