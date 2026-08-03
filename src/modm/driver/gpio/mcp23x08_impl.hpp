/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP23X08_HPP
#	error	"Don't include this file directly, use 'mcp23x08.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < class Transport >
modm::Mcp23x08<Transport>::Mcp23x08(uint8_t address) :
	Transport(address), memory()
{
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::initialize()
{
	memory = Memory(); // reset local register cache
	this->write(i(Register::IOCON), memory.control.value);

	this->write(i(Register::IODIR), memory.direction.value);
	this->write(i(Register::IPOL), memory.polarity.value);
	this->write(i(Register::GPINTEN), memory.interruptEnable.value);
	this->write(i(Register::DEFVAL), memory.interruptDefault.value);
	this->write(i(Register::INTCON), memory.interruptControl.value);
	this->write(i(Register::GPPU), memory.pullup.value);
	this->write(i(Register::GPIO), memory.gpio.value);
	this->write(i(Register::OLAT), memory.outputLatch.value);
	Transport::read(i(Register::INTF), buffer + 7, 4);

	return Transport::read(i(Register::IODIR), buffer, sizeof(buffer));
}

// MARK: - Tasks
template < class Transport >
bool
modm::Mcp23x08<Transport>::setOutput(Pins pins)
{
	// output is 0, input is 1
	memory.direction.reset(pins);

	return this->write(i(Register::IODIR), memory.direction.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::set(Pins pins)
{
	// high is 1, low is 0
	// set output latches locally, but only those that are output
	memory.outputLatch.set(pins & ~memory.direction);

	return this->write(i(Register::GPIO), memory.outputLatch.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::reset(Pins pins)
{
	// high is 1, low is 0
	// reset output latches locally, but only those that are output
	memory.outputLatch.reset(pins & ~memory.direction);

	return this->write(i(Register::GPIO), memory.outputLatch.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::toggle(Pins pins)
{
	// high is 1, low is 0
	// toggle output latches locally, but only those that are output
	memory.outputLatch.toggle(pins & ~memory.direction);

	return this->write(i(Register::GPIO), memory.outputLatch.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::set(Pins pins, bool value)
{
	// high is 1, low is 0
	// update output latches locally, but only those that are output
	memory.outputLatch.update(pins & ~memory.direction, value);

	return this->write(i(Register::GPIO), memory.outputLatch.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::setInput(Pins pins)
{
	// output is 0, input is 1
	memory.direction.set(pins);
	memory.outputLatch.reset(pins);

	return this->write(i(Register::IODIR), memory.direction.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::setPullUp(Pins pins)
{
	// inverted is 1, normal is 0
	memory.pullup.set(pins);

	return this->write(i(Register::GPPU), memory.pullup.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::resetPullUp(Pins pins)
{
	// inverted is 1, normal is 0
	memory.pullup.reset(pins);

	return this->write(i(Register::GPPU), memory.pullup.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::setInvertInput(Pins pins)
{
	// inverted is 1, normal is 0
	memory.polarity.set(pins);

	return this->write(i(Register::IPOL), memory.polarity.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::resetInvertInput(Pins pins)
{
	// inverted is 1, normal is 0
	memory.polarity.reset(pins);

	return this->write(i(Register::IPOL), memory.polarity.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::writePort(PortType data)
{
	// high is 1, low is 0
	// output is 0, input is 1
	// set output latches locally, but only those that are output
	// clear all outputs
	memory.outputLatch.clear(~memory.direction);
	// set masked output values
	memory.outputLatch.set(Pins(data) & ~memory.direction);

	return this->write(i(Register::GPIO), memory.outputLatch.value);
}

template < class Transport >
bool
modm::Mcp23x08<Transport>::readPort(PortType &data)
{
	if (readInput())
	{
		data = memory.gpio.value;
		return true;
	}

	return false;
}
