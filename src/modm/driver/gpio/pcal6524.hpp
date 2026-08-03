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

#pragma once

#include <modm/architecture/interface/gpio_expander.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>

namespace modm
{

/// @ingroup modm_driver_pcal6524
struct pcal6524
{
	/// Wiring options of the single ADDR pin, see Table 4 of the datasheet.
	enum class
	AddressPin : uint8_t
	{
		Scl = 0x20,
		Sda = 0x21,
		Vss = 0x22,
		Vdd = 0x23,
	};

	/// The 24 port pins P0_0 .. P0_7, P1_0 .. P1_7, P2_0 .. P2_7.
	enum class
	Pin : uint32_t
	{
		P0_0 = Bit0,  P0_1 = Bit1,  P0_2 = Bit2,  P0_3 = Bit3,
		P0_4 = Bit4,  P0_5 = Bit5,  P0_6 = Bit6,  P0_7 = Bit7,

		P1_0 = Bit8,  P1_1 = Bit9,  P1_2 = Bit10, P1_3 = Bit11,
		P1_4 = Bit12, P1_5 = Bit13, P1_6 = Bit14, P1_7 = Bit15,

		P2_0 = Bit16, P2_1 = Bit17, P2_2 = Bit18, P2_3 = Bit19,
		P2_4 = Bit20, P2_5 = Bit21, P2_6 = Bit22, P2_7 = Bit23,
	};
	typedef modm::Flags32<Pin> Pins;
	MODM_INT_TYPE_FLAGS(Pins);

protected:
	/// @cond
	/// Start address of each 3-register group (port 0, 1, 2), see Table 6.
	/// A 3-byte transaction starting at one of these addresses accesses all
	/// three ports in one go, since the pointer register auto-increments
	/// through the group by default (AI = 0).
	enum class
	Register : uint8_t
	{
		InputPort0 = 0x00,
		OutputPort0 = 0x04,
		PolarityInversion0 = 0x08,
		Configuration0 = 0x0C,
		PullUpDownEnable0 = 0x4C,
		PullUpDownSelection0 = 0x50,
		InterruptMask0 = 0x54,
		InterruptStatus0 = 0x58,
	};

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond
}; // struct pcal6524

/**
 * PCAL6524 Ultra Low-Voltage Translating 24-bit I2C I/O Expander.
 *
 * A 24-bit general-purpose I/O expander with three 8-bit ports (P0, P1, P2)
 * providing remote I/O expansion via a Fm+ I2C-bus interface up to 1MHz.
 * Unlike simpler expanders, it has independently selectable pull-up *or*
 * pull-down resistors per pin and a maskable, edge- or level-sensitive
 * interrupt output.
 *
 * This driver implements the `modm::GpioExpander` interface plus the most
 * commonly used Agile I/O features (polarity inversion, pull resistors,
 * interrupt mask/status). Output drive strength, input latching, switch
 * debounce, individual open-drain configuration, software reset and the
 * device ID registers are not implemented.
 *
 * @code
 * modm::Pcal6524<MyI2cMaster> expander;
 * expander.initialize();
 * expander.setOutput(pcal6524::Pin::P0_0);
 * expander.set(pcal6524::Pin::P0_0);
 * @endcode
 *
 * @ingroup modm_driver_pcal6524
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Pcal6524 : public pcal6524, public modm::I2cDevice<I2cMaster>, public modm::GpioExpander
{
public:
	static constexpr uint8_t width = 24;
	using PortType = uint32_t;

	static constexpr uint8_t
	indexFromPin(Pin pin)
	{ return modm::leftmostBit(PortType(pin)); }

public:
	/// Constructor.
	/// @param	address	see `pcal6524::AddressPin`, default is ADDR tied to VSS.
	Pcal6524(uint8_t address = uint8_t(AddressPin::Vss)) :
		modm::I2cDevice<I2cMaster>(address), memory()
	{}

public:
	/// Resets the local register cache to the power-up defaults and writes
	/// them out to the device.
	bool
	initialize()
	{
		memory = Memory();
		bool success = write24(Register::Configuration0, memory.direction.value);
		success &= write24(Register::OutputPort0, memory.outputLatch.value);
		success &= write24(Register::PolarityInversion0, memory.polarity.value);
		success &= write24(Register::PullUpDownSelection0, memory.pullSelect.value);
		success &= write24(Register::PullUpDownEnable0, memory.pullEnable.value);
		success &= write24(Register::InterruptMask0, memory.interruptMask.value);
		return success and readInput();
	}

	bool
	setOutput(Pins pins)
	{
		// output is 0, input is 1
		memory.direction.reset(pins);
		return write24(Register::Configuration0, memory.direction.value);
	}

	bool
	set(Pins pins)
	{
		// set output latches locally, but only those that are output
		memory.outputLatch.set(pins & ~memory.direction);
		return write24(Register::OutputPort0, memory.outputLatch.value);
	}

	bool
	reset(Pins pins)
	{
		memory.outputLatch.reset(pins & ~memory.direction);
		return write24(Register::OutputPort0, memory.outputLatch.value);
	}

	bool
	toggle(Pins pins)
	{
		memory.outputLatch.toggle(pins & ~memory.direction);
		return write24(Register::OutputPort0, memory.outputLatch.value);
	}

	bool
	set(Pins pins, bool value)
	{
		memory.outputLatch.update(pins & ~memory.direction, value);
		return write24(Register::OutputPort0, memory.outputLatch.value);
	}

	bool inline
	isSet(Pin pin)
	{ return memory.outputLatch.any(pin); }

	modm::Gpio::Direction inline
	getDirection(Pin pin)
	{
		// output is 0, input is 1
		return memory.direction.any(pin) ?
				modm::Gpio::Direction::In :
				modm::Gpio::Direction::Out;
	}

public:
	bool
	setInput(Pins pins)
	{
		// output is 0, input is 1
		memory.direction.set(pins);
		return write24(Register::Configuration0, memory.direction.value);
	}

	/// Inverts the polarity of the given input pins in the input port register.
	bool
	setInvertInput(Pins pins)
	{
		memory.polarity.set(pins);
		return write24(Register::PolarityInversion0, memory.polarity.value);
	}

	/// Restores the normal (non-inverted) polarity of the given input pins.
	bool
	resetInvertInput(Pins pins)
	{
		memory.polarity.reset(pins);
		return write24(Register::PolarityInversion0, memory.polarity.value);
	}

	bool inline
	read(Pin pin)
	{ return memory.gpio.any(pin); }

	bool
	readInput()
	{
		uint32_t value;
		if (not read24(Register::InputPort0, value)) return false;
		memory.gpio = Pins(value);
		return true;
	}

public:
	bool
	writePort(PortType data)
	{
		// clear all outputs, but only those that are output
		memory.outputLatch.reset(~memory.direction);
		// set masked output values
		memory.outputLatch.set(Pins(data) & ~memory.direction);
		return write24(Register::OutputPort0, memory.outputLatch.value);
	}

	bool
	readPort(PortType &data)
	{
		if (readInput())
		{
			data = memory.gpio.value;
			return true;
		}
		return false;
	}

public:
	Pins inline
	getDirections()
	{ return ~memory.direction; }

	Pins inline
	getOutputs()
	{ return memory.outputLatch; }

	Pins inline
	getInputs()
	{ return memory.gpio; }

	Pins inline
	getPolarities()
	{ return memory.polarity; }

public:
	/// Enables a 100kOhm pull-up resistor on the given pins.
	bool
	setPullUp(Pins pins)
	{
		memory.pullSelect.set(pins);
		if (not write24(Register::PullUpDownSelection0, memory.pullSelect.value))
			return false;
		memory.pullEnable.set(pins);
		return write24(Register::PullUpDownEnable0, memory.pullEnable.value);
	}

	/// Enables a 100kOhm pull-down resistor on the given pins.
	bool
	setPullDown(Pins pins)
	{
		memory.pullSelect.reset(pins);
		if (not write24(Register::PullUpDownSelection0, memory.pullSelect.value))
			return false;
		memory.pullEnable.set(pins);
		return write24(Register::PullUpDownEnable0, memory.pullEnable.value);
	}

	/// Disconnects the pull-up/pull-down resistor from the given pins.
	bool
	disablePull(Pins pins)
	{
		memory.pullEnable.reset(pins);
		return write24(Register::PullUpDownEnable0, memory.pullEnable.value);
	}

	/// Unmasks (enables) the interrupt for the given input pins.
	bool
	setInterrupt(Pins pins)
	{
		memory.interruptMask.reset(pins);
		return write24(Register::InterruptMask0, memory.interruptMask.value);
	}

	/// Masks (disables) the interrupt for the given input pins.
	bool
	maskInterrupt(Pins pins)
	{
		memory.interruptMask.set(pins);
		return write24(Register::InterruptMask0, memory.interruptMask.value);
	}

	/// Reads which input pins caused the last interrupt.
	bool
	readInterruptStatus(Pins &status)
	{
		uint32_t value;
		if (not read24(Register::InterruptStatus0, value)) return false;
		status = Pins(value);
		return true;
	}

public:
	/// Alias-templates for simpler use of the Pin
	/// @{
	template < Pcal6524<I2cMaster> &object >
	using P0_0 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_0 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_1 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_1 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_2 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_2 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_3 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_3 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_4 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_4 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_5 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_5 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_6 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_6 >;
	template < Pcal6524<I2cMaster> &object >
	using P0_7 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P0_7 >;

	template < Pcal6524<I2cMaster> &object >
	using P1_0 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_0 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_1 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_1 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_2 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_2 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_3 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_3 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_4 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_4 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_5 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_5 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_6 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_6 >;
	template < Pcal6524<I2cMaster> &object >
	using P1_7 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P1_7 >;

	template < Pcal6524<I2cMaster> &object >
	using P2_0 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_0 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_1 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_1 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_2 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_2 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_3 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_3 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_4 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_4 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_5 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_5 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_6 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_6 >;
	template < Pcal6524<I2cMaster> &object >
	using P2_7 = GpioExpanderPin< Pcal6524<I2cMaster>, object, Pin::P2_7 >;
	/// @}

	/// Alias-template for simpler use of a range of pins as a port.
	template < Pcal6524<I2cMaster> &object, Pin StartPin, uint8_t Width, GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal >
	using Port = GpioExpanderPort< Pcal6524<I2cMaster>, object, StartPin, Width, DataOrder >;

private:
	// the wire format of a 3-register group is 24bit, so this cannot be
	// overlaid onto the 32bit `Pins::value` directly, unlike the 8/16bit
	// GPIO expanders which pack their command byte and register value into
	// one contiguous, `modm_packed` wire-format buffer.
	bool
	write24(Register reg, uint32_t value)
	{
		buffer[0] = i(reg);
		buffer[1] = uint8_t(value);
		buffer[2] = uint8_t(value >> 8);
		buffer[3] = uint8_t(value >> 16);
		return modm::I2cDevice<I2cMaster>::write(buffer, 4);
	}

	bool
	read24(Register reg, uint32_t &value)
	{
		uint8_t addr = i(reg);
		uint8_t data[3];
		if (not modm::I2cDevice<I2cMaster>::writeRead(&addr, 1, data, 3))
			return false;
		value = uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16);
		return true;
	}

	struct modm_packed
	Memory
	{
		Memory() :
			direction(0xffffff),
			polarity(0),
			pullEnable(0),
			pullSelect(0xffffff),
			interruptMask(0xffffff),
			outputLatch(0xffffff),
			gpio(0)
		{}

		Pins direction;			// Configuration0/1/2
		Pins polarity;			// Polarity Inversion0/1/2
		Pins pullEnable;		// Pull-up/down Enable0/1/2
		Pins pullSelect;		// Pull-up/down Selection0/1/2
		Pins interruptMask;		// Interrupt Mask0/1/2
		Pins outputLatch;		// Output Port0/1/2
		Pins gpio;				// Input Port0/1/2 (buffered readInput() result)
	};

	Memory memory;
	uint8_t buffer[4];
};

} // namespace modm
