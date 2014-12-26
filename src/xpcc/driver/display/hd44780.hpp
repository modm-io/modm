// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HD44780_HPP
#define XPCC_HD44780_HPP

#include <xpcc/architecture/driver.hpp>
#include <xpcc/ui/display/character_display.hpp>
#include "hd44780_base.hpp"

namespace xpcc
{

/**
 * Driver for HD447800 compatible displays
 *
 * This driver supports only one controller, with any configuration up to 80
 * characters (most common: 8x1, 16x2, 20x4).
 * You can choose to use a 4bit bus or a 8bit bus, simply by providing one
 * with the propriate width.
 *
 * @tparam	DATA	A 8bit or 4bit Port.
 * @tparam	RW		Read/Write-Select Pin
 * @tparam	RS		Command/Data-Select Pin
 * @tparam	E		Enable Pin
 *
 * @see		http://en.wikipedia.org/wiki/HD44780_Character_LCD
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	display
 */
template <typename DATA, typename RW, typename RS, typename E>
class Hd44780 : public CharacterDisplay
{
public:
	/// Constructor
	Hd44780(uint8_t width, uint8_t height);

	/**
	 * Initialize the display
	 *
	 * The display needs some time to initialize after startup. You have
	 * to wait at least 50 ms after startup before calling this method.
	 */
	void
	initialize();

	virtual void
	writeRaw(char c);

	virtual void
	setCursor(uint8_t column, uint8_t line);

	void
	execute(Command command);

	void
	clear();

protected:
	typedef Hd44780Base<DATA, RW, RS, E> driver;
};

/**
 * Driver for HD447800 compatible displays with two controller
 *
 * This driver supports two controllers, with any configuration up to 160
 * characters like 40x4.
 * You can choose to use a 4bit bus or a 8bit bus, simply by providing one
 * with the propriate width.
 *
 * @tparam	DATA	A 8bit or 4bit Port.
 * @tparam	RW		Read/Write-Select Pin
 * @tparam	RS		Command/Data-Select Pin
 * @tparam	E1		Enable Pin 1
 * @tparam	E2		Enable Pin 2
 *
 * @see		http://en.wikipedia.org/wiki/HD44780_Character_LCD
 *
 * @author	Niklas Hauser
 * @ingroup	display
 */
template <typename DATA, typename RW, typename RS, typename E1, typename E2>
class Hd44780Dual : public CharacterDisplay
{
public:
	/// Constructor
	Hd44780Dual(uint8_t width, uint8_t height);

	/**
	 * \brief	Initialize the display
	 *
	 * The display needs some time to initialize after startup. You have
	 * to wait at least 50 ms after startup before calling this method.
	 */
	void
	initialize();

	virtual void
	writeRaw(char c);

	virtual void
	setCursor(uint8_t column, uint8_t line);

	void
	execute(Command command);

	void
	clear();

protected:
	typedef Hd44780Base<DATA, RW, RS, E1> driver1;
	typedef Hd44780Base<DATA, RW, RS, E2> driver2;
};

} // namespace xpcc

#include "hd44780_impl.hpp"

#endif // XPCC_HD44780_HPP
