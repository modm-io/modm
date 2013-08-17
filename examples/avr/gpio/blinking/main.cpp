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
typedef xpcc::SoftwareGpioWord<GpioB0, GpioB1, GpioB2, GpioB3, GpioB4, GpioB5, GpioB6> Data4;

/*
#define USE_TEMPLATES

#ifdef USE_TEMPLATES

// Interrupt Handler
template<void (*interruptFunction)(void)>
class UartHandler
{
	static void
	Handler(void) __asm__(STRINGIFY(USARTF0_RXC_vect)) __attribute__((__signal__, __used__, __externally_visible__));
};

template<void (*interruptFunction)(void)>
void
UartHandler<interruptFunction>::Handler(void)
{
	Led::set();
}

void hello() {
	Led::reset();
}

template class UartHandler<hello>;

#else

class UartHandler
{
	static void
	Handler(void) __asm__(STRINGIFY(USARTF0_RXC_vect)) __attribute__((__signal__, __used__, __externally_visible__));
};

void
UartHandler::Handler(void)
{
	Led::set();
}
#endif//*/

int
main(void)
{
	Data3::setOutput();

	volatile uint8_t write = 0xff;
	volatile uint8_t read = 0;

	Data3::write(write);
	Data4::write(write);

	read = Data3::read();
	read = Data4::read();
	(void) read;

	while(1)
		;
}

