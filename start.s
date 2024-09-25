interrupt_vector_table:
    b _start @ Reset Handler
    b . @ Undefined
    b swi_handler @ SWI Handler
    b . @ Prefetch Abort
    b data_abort @ Data Abort
    b . /* 0x14 Reserved */
    b irq_handler @ IRQ

.global _start
_start:
    msr cpsr_c, 0x12
    ldr sp, =_irq_stack_end

    msr cpsr_c, 0x13
    ldr sp, =_stack_end
    bl start
1:
    b 1b
data_abort:
    swi 0xFF

.size _start, . - _start