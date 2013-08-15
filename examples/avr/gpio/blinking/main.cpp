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
typedef GpioOutputB1 Led2;

typedef xpcc::GpioInverted< Led2 > LedInverted;

typedef GpioPortD<0, 8> Data;
typedef GpioPortD<2, 5> Data2;
typedef xpcc::SoftwareGpioOctet<GpioC1, GpioC4, GpioB6, GpioB3, GpioB5> Data3;

int
main(void)
{
	Led::setOutput();
	Led::set();

	Led2::setOutput();

	Data::setOutput();
	Data::write(0x0f);

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
