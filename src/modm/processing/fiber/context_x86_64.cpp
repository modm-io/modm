/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "context.hpp"

modm_context
modm_makecontext(modm_stack_t* stack, std::size_t stack_size,
                 void (*fn)(void), void (*end)(void)) {
  modm_stack_t* sp = stack + stack_size / sizeof(modm_stack_t);
  *--sp = (modm_stack_t) end; // Called when fn returns.
  *--sp = (modm_stack_t) fn;
  sp -= 6; // r12-15,rbx,rbp
  return {sp, stack_size};
}

void
modm_startcontext(const modm_context &to) {
  modm_jumpcontext(&main_context, to);
}

void
modm_jumpcontext(modm_context* from, const modm_context &to) {
  register modm_stack_t** from_sp asm("rcx") = &from->sp;
  register modm_stack_t* to_sp asm("rdx") = to.sp;
  asm volatile(
    "call 1f\n\t" // compute abs address of label 1
    "1: popq %%rax\n\t"
    "addq $(2f-1b), %%rax\n\t" // Add offset to label 2 to RAX
    "pushq %%rax\n\t" // Push abs address of label 2 to stack

    "pushq %%rbp\n\t"
    "pushq %%rbx\n\t"
    "pushq %%r15\n\t"
    "pushq %%r14\n\t"
    "pushq %%r13\n\t"
    "pushq %%r12\n\t"

    "movq %%rsp, (%[from_sp])\n\t" // Store the SP in "from"
    "movq %[to_sp], %%rsp\n\t" // Restore SP from "to"

    "popq %%r12\n\t"
    "popq %%r13\n\t"
    "popq %%r14\n\t"
    "popq %%r15\n\t"
    "popq %%rbx\n\t"
    "popq %%rbp\n\t"

    "ret\n\t" // Perform the jump
    "2:\n\t"
    /*outputs*/: [from_sp] "+r" (from_sp), [to_sp] "+r" (to_sp)
    /*inputs*/:
    /*clobbers*/: "rax", "memory");
}

void
modm_endcontext() {
  modm_context dummy;
  modm_jumpcontext(&dummy, main_context);
}
