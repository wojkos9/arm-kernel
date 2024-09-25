#include "gic.h"
#include "cpu.h"
#include "printf.h"

// static gic_distributor_registers* gic_dregs;
// static gic_cpu_interface_registers* gic_ifregs;

void gic_init(void) {
    gic_cpu_interface_registers* gic_ifregs = (gic_cpu_interface_registers*)GIC_IFACE_BASE;
    gic_distributor_registers* gic_dregs = (gic_distributor_registers*)GIC_DIST_BASE;

    WRITE32(gic_dregs->DCTLR, 1); /* Enable the interrupt distributor */

    WRITE32(gic_ifregs->CCPMR, 0xffff);
    WRITE32(gic_ifregs->CCTLR, CCTRL_ENABLE); /* Enable interrupt forwarding to this CPU */


}



void gic_enable_interrupt(uint16_t number) {
    gic_cpu_interface_registers* gic_ifregs = (gic_cpu_interface_registers*)GIC_IFACE_BASE;
    gic_distributor_registers* gic_dregs = (gic_distributor_registers*)GIC_DIST_BASE;
    /* Enable the interrupt */
    uint8_t reg = number / 32;
    uint8_t bit = number % 32;

    uint32_t reg_val = gic_dregs->DISENABLER[reg];
    printf("reg %08x %d\n", reg_val, number);
    reg_val |= (1u << bit);
    WRITE32(gic_dregs->DISENABLER[reg], reg_val);

    // /* Forward interrupt to CPU Interface 0 */
    // reg = number / 4;
    // bit = (number % 4) * 8; /* Can result in bit 0, 8, 16 or 24 */
    // reg_val = gic_dregs->DITARGETSR[reg];
    // reg_val |= (2u << bit);
    // WRITE32(gic_dregs->DITARGETSR[reg], reg_val);
}

uint16_t gic_acknowledge_interrupt(void) {
    gic_cpu_interface_registers* gic_ifregs = (gic_cpu_interface_registers*)GIC_IFACE_BASE;
    return gic_ifregs->CIAR & CIAR_ID_MASK;
}

void gic_end_interrupt(uint16_t number) {
    gic_cpu_interface_registers* gic_ifregs = (gic_cpu_interface_registers*)GIC_IFACE_BASE;
    WRITE32(gic_ifregs->CEOIR, (number & CEOIR_ID_MASK));
}