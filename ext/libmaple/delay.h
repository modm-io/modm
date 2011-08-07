/**
 * @file delay.h
 * @brief Delay implementation
 */

#ifndef _DELAY_H_
#define _DELAY_H_

/**
 * @brief Delay the given number of microseconds.
 *
 * Note that this function currently assumes you are on a LeafLabs
 * board, and will only work properly if you follow the LeafLabs RCC
 * configuration.
 *
 * @param us Number of microseconds to delay.
 */
static inline void delay_us(uint32 us) {
    /* TODO this makes unwarranted assumptions about the RCC
     * config; add a hook so users can make their own decisions. */
    /* So (2^32)/12 micros max, or less than 6 minutes */
    us *= 12;

    /* fudge for function call overhead  */
    us--;
    asm volatile("   mov r0, %[us]          \n\t"
                 "1: subs r0, #1            \n\t"
                 "   bhi 1b                 \n\t"
                 :
                 : [us] "r" (us)
                 : "r0");
}
#endif

