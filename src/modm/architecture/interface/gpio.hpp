/*
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GPIO_HPP
#define MODM_GPIO_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

namespace modm
{

/// @ingroup modm_architecture_gpio
struct Gpio
{
	/**
	 * These constants refer to the *logical* state of the GPIO.
	 * The physical state is determined by the configuration and external connection.
	 */
	static constexpr bool Low = false;
	static constexpr bool High = true;

	/// Declares the direction of a GPIO
	enum class
	Direction
	{
		In = 0,			//< GPIO is an Input
		Out = 1,		//< GPIO is an Output
		InOut = 2,		//< GPIO is both Input and Output
		Special = 3,	//< GPIO is configured with a special purpose
	};
};

/**
 * Input interface of an I/O pin.
 *
 * The pin is not automatically set as an input after startup,
 * so remember to call `setInput()` after startup!
 * @ingroup modm_architecture_gpio
 */
class GpioInput : public Gpio
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::In;

#ifdef __DOXYGEN__
	/// configure pin as input
	static void
	setInput();

	/// read input
	static bool
	read();
#endif
};

/**
 * Output interface of an I/O pin.
 *
 * The pin is not automatically set as an output after startup,
 * so remember to call `setOutput()` after startup!
 * @ingroup modm_architecture_gpio
 */
class GpioOutput : public Gpio
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::Out;

#ifdef __DOXYGEN__
	/// configure pin as output
	static void
	setOutput();

	/// configure pin as output and set high or low
	static void
	setOutput(bool value);

	/// set output to high level
	static void
	set();

	/// set output to high or low level
	static void
	set(bool value);

	/// set output to low level
	static void
	reset();

	/// toggle output level
	static void
	toggle();

	/// Returns the set logical output state of the pin.
	/// This may be different from the physical state!
	static bool
	isSet();
#endif
};

/**
 * Input/Output interface of an I/O pin.
 * @ingroup modm_architecture_gpio
 */
class GpioIO : public GpioOutput, public GpioInput
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::InOut;

#ifdef __DOXYGEN__
	/// returns the pin direction at run-time
	static Direction
	getDirection();
#endif
};

/**
 * Input/output interface of a set of up to 16 I/O pins.
 *
 * This class provides the interface for up to 16 parallel IOs.
 * Be aware that the interface is the same regardless of the actual
 * mapping of the IOs.
 * For example, when a high octet `0xf400` is physically configured, the
 * data will still be written and read as a low octet `0x00f4`.
 * @ingroup modm_architecture_gpio
 */
class GpioPort
{
public:
	/// This enum can be used to reverse the bit order of the Data.
	enum class
	DataOrder : bool
	{
		Normal = false, ///< bit order of data is the same as bit order of port
		Reversed = true ///< bit order of data is the reverse bit order of port
	};
#ifdef __DOXYGEN__
	/// This holds the width of the port as number of bits
	/// and can be used in drivers to assert the correct requirements
	static constexpr uint8_t width;

	/// This holds the number of unique platform ports that this
	/// implementation accesses. It's always at least 1.
	static constexpr uint8_t number_of_ports;

	/// may also be uint8_t, depending on port width
	using PortType = uint16_t;

	/// returns the chosen data order
	static constexpr DataOrder
	getDataOrder();

public:
	static void
	setOutput();

	static void
	setInput();

	/// The read data is always right-aligned regardless of physical mapping.
	static PortType
	read();

	/// The write data is always right-aligned regardless of physical mapping.
	static void
	write(PortType data);

	/// Toggle the output states of all pins
	static void
	toggle();
#endif
};

} // namespace modm

#endif // MODM_GPIO_HPP
