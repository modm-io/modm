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

typedef GpioPortD<0, 8> Data;
typedef GpioPortD<2, 5> Data3;
typedef xpcc::SoftwareGpioOctet<GpioC1, GpioC4, GpioB6, GpioB3, GpioB5> Data2;

static_assert(Data::width == 8, "Data::width is not 8");
static_assert(Data2::width == 5, "Data2::width is not 5");

typedef SimpleSpi SPI;
typedef UartSimpleSpi0 SPI2;
typedef Uart0 UART;

MAIN_FUNCTION
//int
//main(void)
{
	Led::setOutput();
	Led::set();
	Data::setOutput();
	volatile uint8_t datat = 0x0f;
	Data::write(datat);
	Data3::write(datat);

	Data2::setOutput();
	datat = 0x1e;
	Data2::write(datat);

	LedInverted::setOutput();
	LedInverted::reset();

	GpioD0::connect(Uart0::Id);
	GpioD1::connect(Uart0::Id);

	SPI::initialize<2000000>(SimpleSpi::Mode::Mode3);
	SPI2::initialize<1000000>(SimpleSpi::Mode::Mode0);
	UART::initialize<38400>();

	while (1)
	{
	}
}
