#include <stdint.h>
#include "cpu.h"
#include "pl031.h"
#include "printf.h"

#include "gic.h"

#include "virt.h"

#define UART0_BASE 0x9000000
#define UART0INT (33u)
#define UART_TXFF 0x20
#define UART_RXFE 0x10

#define CR_UARTEN       (1 << 0u)
#define IMSC_RXIM	(1u << 4u)

#define BE_INTERRUPT	(1u << 9u)

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
	uint32_t icr;				/* 0x44 Interrupt clear register */
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

void swi_handler() {
    printf("data abort\n");
    for(;;);
}

void pl011_isr() {
    uint32_t status = uart0->mis;
    if (status & IMSC_RXIM) {
        printf("char %d val %d\n", try_getc(), rtc->dr);
    }

    uart0->icr = 0x7ff;
}

void timer_en(unsigned time_ms) {
    uint32_t reg32;
    uint32_t frq;
    __asm__ __volatile__ ("mrc p15, 0, %0, c14, c0, 0" : "=r"(frq));


    reg32 = frq / 1000 * time_ms;
    __asm__ __volatile__ ("mcr p15, 0, %0, c14, c2, 0" : "=r"(reg32));

    reg32 = 1;
    __asm__ __volatile__ ("mcr p15, 0, %0, c14, c2, 1" : "=r"(reg32));
}

void timer_check() {
    int32_t reg32;
    __asm__ __volatile__ ("mrc p15, 0, %0, c14, c2, 0" : "=r"(reg32));

    uint32_t frq;
    __asm__ __volatile__ ("mrc p15, 0, %0, c14, c0, 0" : "=r"(frq));
    printf("clk val %d frq %d\n", reg32, frq);
}

void rtc_isr() {
    timer_check();
    rtc->icr = 1;
    rtc->lr = 0;
}

void timer_isr() {
    uint32_t reg32;
    __asm__ __volatile__ ("mrc p15, 0, %0, c14, c2, 1" : "=r"(reg32));
    printf("status %x\n", reg32);

    reg32 = 2;
    __asm__ __volatile__ ("mcr p15, 0, %0, c14, c2, 1" : "=r"(reg32));

    timer_en(100);
}

void __attribute__((interrupt)) irq_handler() {
    gic_cpu_interface_registers *ifregs = (gic_cpu_interface_registers *)GIC_IFACE_BASE;
    uint32_t irq = ifregs->CIAR & CIAR_ID_MASK;
    switch(irq) {
        case UART0INT:
            pl011_isr();
            break;
        case RTCIRQ:
            rtc_isr();
            break;
        case 30:
            timer_isr();
            break;
        default:
            printf("unknown irq %u\n", irq);
            break;
    }
    WRITE32(ifregs->CEOIR, irq & CEOIR_ID_MASK);
}

char c;

int gic_get_nr() {
    gic_distributor_registers *dregs = (gic_distributor_registers *)GIC_DIST_BASE;
    int irqs = dregs->DTYPER & 0x1f;
    // irqs = (irqs + 1) * 32;
    return irqs;
}

void setup_uart() {
    uart0->cr &= ~CR_UARTEN;
    uart0->imsc |= IMSC_RXIM;
    uart0->cr |= CR_UARTEN;
}

void rtc_init() {
    rtc->cr = 0;

    rtc->mr = 1;
    rtc->lr = 0;

    rtc->imsc = 1;
    rtc->cr = 1;
}

void timer_init() {
    uint32_t reg32;
    // reg32 = 1000000000;
    // __asm__ __volatile__ ("mcr p15, 0, %0, c14, c0, 0" : "=r"(reg32));

    reg32 = 2;
    __asm__ __volatile__ ("mcr p15, 0, %0, c14, c2, 1" : "=r"(reg32));
}





void start() {
    gic_distributor_registers *dregs = (gic_distributor_registers *)GIC_DIST_BASE;
    __asm__ __volatile__("cpsid if");

    setup_uart();
    rtc_init();
    // timer_init();
    gic_init();

    // int irq = 33;
    // uint32_t mask = 1 << 1;
    // int reg = 1;
    // WRITE32(dregs->DISENABLER[reg], mask);

    gic_enable_interrupt(33);
    gic_enable_interrupt(RTCIRQ);

    // timer
    gic_enable_interrupt(30);

    __asm__ __volatile__("cpsie if");

    timer_en(100);

    putc('h');
    putc('\r');
    putc('\n');

    for(;;) {
        __asm__ __volatile__ ("wfi");
    }

    // printf("hello\n");
    // for(;;) {
    //     char c = try_getc();
    //     if (c != 0) {
    //         _putchar(c);
    //     }
    // }
}


