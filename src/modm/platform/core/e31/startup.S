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
    la t0, lowlevel_vector
    csrw mtvec, t0

    #enable the interrupts
    #li t0, 0x08
    #csrw mstatus, t0

    #reset and enable the rtc for use as time base
    #move that into a hardware init c function
    li t0, 0x10000040   #RTC Base register
    sw zero, 0x08(t0)   #Reset the Counter Low Register
    sw zero, 0x0C(t0)   #Reset the Counter High Register
    li t1,   0x1000     #set the bit for the always on reg
    sw t1,   0x00(t0)   #and store it

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
.align
lowlevel_vector:
    .cfi_startproc
    csrr a0, mcause
    #call _trapVector
    mret
    .cfi_endproc

.global __register_frame_info
.section .text.init.__register_frame_info
__register_frame_info:
    .cfi_startproc
    ret
    .cfi_endproc
