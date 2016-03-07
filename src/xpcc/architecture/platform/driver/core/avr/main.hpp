// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AVR_MAIN_HPP
#define	XPCC_AVR_MAIN_HPP

#include <stdint.h>

// saves a couple of bytes of stack and program.
int main(void) __attribute__((OS_main));

#endif	// XPCC_AVR_MAIN_HPP
