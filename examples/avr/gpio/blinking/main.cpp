// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
#include <xpcc/architecture.hpp>

using namespace xpcc::xmega;

typedef GpioOutputB0 Led;
typedef xpcc::SoftwareGpioPort<GpioB6, GpioB5, GpioB4, GpioB3, GpioB2, GpioB1, GpioB0> Data3;

int
main(void)
{
	Data3::setOutput();

	volatile uint16_t write = 0xaaaa;
	volatile uint16_t read = 0;

	Data3::write(write);

	read = Data3::read();
	(void) read;

	while(1)
		;
}

