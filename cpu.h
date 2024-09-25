#ifndef CPU_A9_H
#define CPU_A9_H

#include <stdint.h>

#define WRITE32(_reg, _val) (*(volatile uint32_t*)&_reg = _val)

#define GIC_IFACE_OFFSET        (0x2000)
#define GIC_DISTRIBUTOR_OFFSET  (0x1000)

uint32_t cpu_get_periphbase(void);
void cpu_enable_interrupts(void);



#endif