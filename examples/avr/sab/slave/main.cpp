/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013, 2015-2018, Niklas Hauser
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

#include <modm/platform.hpp>
#include <modm/communication/sab/slave.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

// ----------------------------------------------------------------------------
// wrapper class for the A/D converter
class AnalogDigital : public modm::sab::Callable
{
public:
	AnalogDigital()
	{
		// initialize the analog to digital converter
		Adc::initialize<systemClock, 115000>();
		Adc::setReference(Adc::Reference::InternalVcc);
	}

	void
	readChannel(modm::sab::Response& response, const uint8_t *channel)
	{
		uint16_t value = Adc::readChannel(*channel);
		response.send(value);
	}
};

// ----------------------------------------------------------------------------
// wrapper for PORTD
class InOut : public modm::sab::Callable
{
public:
	void
	setDirection(modm::sab::Response& response, const uint8_t *direction)
	{
		DDRD = *direction;

		// send a response without any attached data
		response.send();
	}

	void
	readInput(modm::sab::Response& response)
	{
		uint8_t value = PIND;
		response.send(value);
	}

	void
	setOutput(modm::sab::Response& response, const uint8_t *output)
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
// FIXME: Use FLASH_STORAGE(modm::sab::Action actionList[])
const modm::sab::Action actionList[] =
{
	SAB_ACTION( 'A', analogDigital,	AnalogDigital::readChannel,	1 ),
	SAB_ACTION( 'D', inOut,			InOut::setDirection,		1 ),
	SAB_ACTION( 'I', inOut,			InOut::readInput,			0 ),
	SAB_ACTION( 'O', inOut,			InOut::setOutput,			1 ),
};

// wrap the type definition inside a typedef to make the code more readable
typedef modm::sab::Slave< modm::sab::Interface< Uart0 > > Slave;

// ----------------------------------------------------------------------------
int
main()
{
	// initialize ABP interface, set baudrate etc.
	Slave slave(0x02,
			modm::accessor::asFlash(actionList),
			sizeof(actionList) / sizeof(modm::sab::Action));

	// enable interrupts
	enableInterrupts();

	while (1)
	{
		// decode received messages etc.
		slave.update();
	}
}
