interrupt_vector_table:
    b . @ Reset Handler
    b . @ Undefined
    b . @ SWI Handler
    b . @ Prefetch Abort
    b . @ Data Abort
    b irq_handler @ IRQ
    b . @ FIQ

.global _start
_start:
    ldr sp, =STACK_TOP
    bl start
1:
    b 1b
.size _start, . - _start