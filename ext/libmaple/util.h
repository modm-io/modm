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
 * @file util.h
 * @brief Miscellaneous utility macros and procedures.
 */

#include "libmaple_types.h"

#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Bit manipulation
 */

#define BIT(shift)                     (1UL << (shift))
#define BIT_MASK_SHIFT(mask, shift)    ((mask) << (shift))
/** Bits m to n of x */
#define GET_BITS(x, m, n) ((((uint32)x) << (31 - (n))) >> ((31 - (n)) + (m)))
#define IS_POWER_OF_TWO(v)  (v && !(v & (v - 1)))

/*
 * Failure routines
 */

void __error(void);
void _fail(const char*, int, const char*);
void throb(void);

/*
 * Asserts and debug levels
 */

#define DEBUG_NONE      0
#define DEBUG_FAULT     1
#define DEBUG_ALL       2

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_ALL
#endif

#if DEBUG_LEVEL >= DEBUG_ALL
#define ASSERT(exp)                              \
    if (exp) {                                   \
    } else {                                     \
        _fail(__FILE__, __LINE__, #exp);         \
    }
#else
#define ASSERT(exp) (void)((0))
#endif

#if DEBUG_LEVEL >= DEBUG_FAULT
#define ASSERT_FAULT(exp)                       \
    if (exp) {                                  \
    } else {                                    \
        _fail(__FILE__, __LINE__, #exp);        \
    }
#else
#define ASSERT_FAULT(exp) (void)((0))
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
