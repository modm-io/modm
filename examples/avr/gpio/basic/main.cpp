// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB0 Led;
typedef GpioOutputB1 Led2;

typedef xpcc::GpioInverted< Led2 > LedInverted;

typedef GpioPortD<4, 4> Data;
typedef xpcc::SoftwareGpioOctet<GpioC1, GpioC4, GpioB6, GpioB3, GpioB5> Data2;

static_assert(Data::width == 4, "Data::width is not 4");
static_assert(Data2::width == 5, "Data2::width is not 5");

int
main(void)
{
	Led::setOutput();
	Led::set();
	Data::setOutput();
	Data::write(0x0f);

	GpioD0::connect(Uart0::Id);
	GpioD1::connect(Uart0::Id);

	Data2::setOutput();
	Data2::write(0x1f);

	LedInverted::setOutput();
	LedInverted::reset();

	while (1)
	{
	}
}
