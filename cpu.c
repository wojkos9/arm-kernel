#include "cpu.h"

uint32_t cpu_get_periphbase(void) {
    uint32_t result;
    __asm__ __volatile__ ("mrc p15, #4, %0, c15, c0, #0" : "=r" (result));
    return result;
}

void cpu_enable_interrupts(void) {
    __asm__ __volatile__ ("cpsie if");
}