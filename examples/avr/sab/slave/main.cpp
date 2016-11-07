/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * Example for a simple IO extension
 *
 * This program enables a master to use the A/D converter and PORTD
 * from extern.
 */

#include <xpcc/architecture/platform.hpp>
#include <xpcc/communication/sab/slave.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// ----------------------------------------------------------------------------
// wrapper class for the A/D converter
class AnalogDigital : public xpcc::sab::Callable
{
public:
	AnalogDigital()
	{
		// initialize the analog to digital converter
		Adc::initialize<clock, 115000>();
		Adc::setReference(Adc::Reference::InternalVcc);
	}

	void
	readChannel(xpcc::sab::Response& response, const uint8_t *channel)
	{
		uint16_t value = Adc::readChannel(*channel);
		response.send(value);
	}
};

// ----------------------------------------------------------------------------
// wrapper for PORTD
class InOut : public xpcc::sab::Callable
{
public:
	void
	setDirection(xpcc::sab::Response& response, const uint8_t *direction)
	{
		DDRD = *direction;

		// send a response without any attached data
		response.send();
	}

	void
	readInput(xpcc::sab::Response& response)
	{
		uint8_t value = PIND;
		response.send(value);
	}

	void
	setOutput(xpcc::sab::Response& response, const uint8_t *output)
	{
		PORTD = *output;
		response.send();
	}
};

// ----------------------------------------------------------------------------
// create instances from the wrapper classes
AnalogDigital analogDigital;
InOut inOut;

// ----------------------------------------------------------------------------
// create a list of all possible actions
FLASH_STORAGE(xpcc::sab::Action actionList[]) =
{
	SAB__ACTION( 'A', analogDigital,	AnalogDigital::readChannel,	1 ),
	SAB__ACTION( 'D', inOut,			InOut::setDirection,		1 ),
	SAB__ACTION( 'I', inOut,			InOut::readInput,			0 ),
	SAB__ACTION( 'O', inOut,			InOut::setOutput,			1 ),
};

// wrap the type definition inside a typedef to make the code more readable
typedef xpcc::sab::Slave< xpcc::sab::Interface< Uart0 > > Slave;

// ----------------------------------------------------------------------------
int 
main()
{
	// initialize ABP interface, set baudrate etc.
	Slave slave(0x02,
			xpcc::accessor::asFlash(actionList),
			sizeof(actionList) / sizeof(xpcc::sab::Action));

	// enable interrupts
	enableInterrupts();

	while (1)
	{
		// decode received messages etc.
		slave.update();
	}
}
