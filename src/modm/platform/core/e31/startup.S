.section .text.init.entry
.global _entry
_entry:
    .cfi_startproc
    .cfi_undefined ra

.option push
.option norelax
    la gp, __global_pointer$
.option pop

    # init the stack pointer
    la sp, _sp

    #set up the isr for the trap interrupt
    #TODO: set this to a proper ISR
    la t0, dummy_isr
    csrw mtvec, t0

    #TODO: enable the interrupts here

    #init the c runtime
    csrr a0, mhartid
    li a1, 0
    li a2, 0
    call _start

# we should never reach this point
fault_loop:
    j fault_loop

    .cfi_endproc

#this dummy loop wil
.balign 8
dummy_isr:
    .cfi_startproc
    csrr t0, mcause
    csrr t1, mepc
    csrr t2, mtval
    j dummy_isr
    .cfi_endproc

.global __register_frame_info
.section .text.init.__register_frame_info
__register_frame_info:
    .cfi_startproc
    ret
    .cfi_endproc
