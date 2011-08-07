/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 * @file exti.c
 * @brief External interrupt control routines
 */

#include "libmaple.h"
#include "exti.h"
#include "nvic.h"
#include "bitband.h"

/*
 * Internal state
 */

/* Status bitmaps, for external interrupts with multiplexed IRQs */
static uint16 exti_9_5_en = 0;
static uint16 exti_15_10_en = 0;

typedef struct exti_channel {
    void (*handler)(void);
    uint32 irq_line;
} exti_channel;

static exti_channel exti_channels[] = {
    { .handler = NULL, .irq_line = NVIC_EXTI0      },  // EXTI0
    { .handler = NULL, .irq_line = NVIC_EXTI1      },  // EXTI1
    { .handler = NULL, .irq_line = NVIC_EXTI2      },  // EXTI2
    { .handler = NULL, .irq_line = NVIC_EXTI3      },  // EXTI3
    { .handler = NULL, .irq_line = NVIC_EXTI4      },  // EXTI4
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI5
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI6
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI7
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI8
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI9
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI10
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI11
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI12
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI13
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI14
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI15
};

/*
 * Convenience routines
 */

static inline void enable_irq(afio_exti_num exti_num);
static inline void maybe_disable_irq(afio_exti_num exti_num);

/**
 * @brief Register a handler to run upon external interrupt.
 *
 * This function assumes that the interrupt request corresponding to
 * the given external interrupt is masked.
 *
 * @param num     External interrupt line number.
 * @param port    Port to use as source input for external interrupt.
 * @param handler Function handler to execute when interrupt is triggered.
 * @param mode    Type of transition to trigger on, one of:
 *                EXTI_RISING, EXTI_FALLING, EXTI_RISING_FALLING.
 * @see exti_num
 * @see exti_port
 * @see exti_trigger_mode
 */
void exti_attach_interrupt(afio_exti_num num,
                           afio_exti_port port,
                           voidFuncPtr handler,
                           exti_trigger_mode mode) {
    ASSERT(handler);

    /* Register the handler */
    exti_channels[num].handler = handler;

    /* Set trigger mode */
    switch (mode) {
    case EXTI_RISING:
        *bb_perip(&EXTI_BASE->RTSR, num) = 1;
        break;
    case EXTI_FALLING:
        *bb_perip(&EXTI_BASE->FTSR, num) = 1;
        break;
    case EXTI_RISING_FALLING:
        *bb_perip(&EXTI_BASE->RTSR, num) = 1;
        *bb_perip(&EXTI_BASE->FTSR, num) = 1;
        break;
    }

    /* Map num to port */
    afio_exti_select(num, port);

    /* Unmask external interrupt request */
    *bb_perip(&EXTI_BASE->IMR, num) = 1;

    /* Enable the interrupt line */
    enable_irq(num);
}

/**
 * @brief Unregister an external interrupt handler
 * @param num Number of the external interrupt line to disable.
 * @see exti_num
 */
void exti_detach_interrupt(afio_exti_num num) {
    /* First, mask the interrupt request */
    *bb_perip(&EXTI_BASE->IMR, num) = 0;

    /* Then, clear the trigger selection registers */
    *bb_perip(&EXTI_BASE->FTSR, num) = 0;
    *bb_perip(&EXTI_BASE->RTSR, num) = 0;

    /* Next, disable the IRQ, unless it's multiplexed and there are
     * other active external interrupts on the same IRQ line */
    maybe_disable_irq(num);

    /* Finally, unregister the user's handler */
    exti_channels[num].handler = NULL;
}

/*
 * Interrupt handlers
 */

static inline void clear_pending(uint32 exti_num);
static inline void dispatch_handler(uint32 exti_num);

/* For AFIO_EXTI_0 through AFIO_EXTI_4, only one handler is associated
 * with each channel, so we don't have to keep track of which channel
 * we came from */
void __irq_exti0(void) {
    dispatch_handler(AFIO_EXTI_0);
    clear_pending(AFIO_EXTI_0);
}

void __irq_exti1(void) {
    dispatch_handler(AFIO_EXTI_1);
    clear_pending(AFIO_EXTI_1);
}

void __irq_exti2(void) {
    dispatch_handler(AFIO_EXTI_2);
    clear_pending(AFIO_EXTI_2);
}

void __irq_exti3(void) {
    dispatch_handler(AFIO_EXTI_3);
    clear_pending(AFIO_EXTI_3);
}

void __irq_exti4(void) {
    dispatch_handler(AFIO_EXTI_4);
    clear_pending(AFIO_EXTI_4);
}

void __irq_exti9_5(void) {
    /* Figure out which channel it came from  */
    uint32 pending = GET_BITS(EXTI_BASE->PR, 5, 9);
    uint32 i;

    /* Dispatch every handler if the pending bit is set */
    for (i = 0; i < 5; i++) {
        if (pending & 0x1) {
            dispatch_handler(AFIO_EXTI_5 + i);
            clear_pending(AFIO_EXTI_5 + i);
        }
        pending >>= 1;
    }
}

void __irq_exti15_10(void) {
    /* Figure out which channel it came from  */
    uint32 pending = GET_BITS(EXTI_BASE->PR, 10, 15);
    uint32 i;

    /* Dispatch every handler if the pending bit is set */
    for (i = 0; i < 6; i++) {
        if (pending & 0x1) {
            dispatch_handler(AFIO_EXTI_10 + i);
            clear_pending(AFIO_EXTI_10 + i);
        }
        pending >>= 1;
    }
}

/*
 * Auxiliary functions
 */

static inline void clear_pending(uint32 exti_num) {
    *bb_perip(&EXTI_BASE->PR, exti_num) = 1;
    /* If the pending bit is cleared as the last instruction in an ISR,
     * it won't actually be cleared in time and the ISR will fire again.
     * Insert a 2-cycle buffer to allow it to take effect. */
    asm volatile("nop");
    asm volatile("nop");
}

static inline void dispatch_handler(uint32 exti_num) {
    ASSERT(exti_channels[exti_num].handler);
    if (exti_channels[exti_num].handler) {
        (exti_channels[exti_num].handler)();
    }
}

static inline void enable_irq(afio_exti_num exti) {
    /* Maybe twiddle the IRQ bitmap for extis with multiplexed IRQs */
    if (exti > 4) {
        uint16 *bitmap = exti < 10 ? &exti_9_5_en : &exti_15_10_en;
        *bb_sramp(bitmap, exti) = 1;
    }

    nvic_irq_enable(exti_channels[exti].irq_line);
}

static inline void maybe_disable_irq(afio_exti_num exti) {
    if (exti > 4) {
        uint16 *bitmap = exti < 10 ? &exti_9_5_en : &exti_15_10_en;
        *bb_sramp(bitmap, exti) = 0;
        if (*bitmap == 0) {
            /* All of the external interrupts which share this IRQ
             * line are disabled. */
            nvic_irq_disable(exti_channels[exti].irq_line);
        }
    } else {
        nvic_irq_disable(exti_channels[exti].irq_line);
    }
}
