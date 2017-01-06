/*
 * Copyright (c) 2013-2014, 2016, Niklas Hauser
 * Copyright (c) 2014-2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HD44780_BASE_HPP
#define MODM_HD44780_BASE_HPP

namespace modm
{

/**
 * Base driver for one HD447800 compatible display driver.
 *
 * An HD44780 Character LCD is a de-facto industry standard liquid crystal
 * display (LCD) display device. They can drive up to 80 characters with one
 * driver, and more with additional ones, selected by additional Enable lines.
 *
 * This driver can handle 4-Bit or 8-Bit bus mode with read back.
 * For either data bus width the class will internally choose the right
 * bus implementation, so the interface remains exactly the same.
 * You also need to connect all the control pins (RW, RS, E).
 *
 * @tparam	DATA	A 8bit or 4bit Port.
 * @tparam	RW		Read/Write-Select Pin
 * @tparam	RS		Command/Data-Select Pin
 * @tparam	E		Enable Pin
 *
 * @see		http://en.wikipedia.org/wiki/HD44780_Character_LCD
 *
 * @author	Niklas Hauser
 * @ingroup	lcd
 */
template <typename DATA, typename RW, typename RS, typename E>
class Hd44780Base
{
	enum
	InternalCommand
	{
		ClearDisplay		= 0b00000001,	///< Clear the display content
		ResetCursor			= 0b00000010,	///< Set cursor to position (0,0)

		IncrementCursor		= 0b00000110,	///< Increments address upon write
		DecrementCursor		= 0b00000100,	///< Decrements address upon write

		DisableDisplay		= 0b00001000,	///< Disable display
		EnableDisplay		= 0b00001100,	///< Enable display
		DisableCursor		= 0b00001000,	///< Disable cursor
		EnableCursor		= 0b00001010,	///< Enable cursor
		DisableCursorBlink	= 0b00001000,	///< Disable blinking cursor
		EnableCursorBlink	= 0b00001001,	///< Enable blinking cursor

		ShiftCursorLeft		= 0b00010000,	///< Shift cursor left
		ShiftCursorRight	= 0b00010100,	///< Shift cursor right

		Set1LineDisplay		= 0b00100000,	///< Enables 1 Line mode
		Set2LineDisplay		= 0b00101000,	///< Enables 2 Line mode
		Set4BitBus			= 0b00100000,	///< Enables 4bit bus mode
		Set8BitBus			= 0b00110000,	///< Enables 8bit bus mode

		SetCGRAM_Address	= 0b01000000,
		SetDDRAM_Address	= 0b10000000,
	};

public:
	enum class
	LineMode
	{
		OneLine = Set1LineDisplay,
		TwoLines = Set2LineDisplay
	};

	/// initialize the display to the current bus and line mode
	/// @warning	you need to wait more than 30ms after Vcc rises to 4.5V
	///				before calling this function
	static void
	initialize(LineMode lineMode);

	/// Clear the display of all Data
	/// @return	`true` if operation successful, `false` if controller is busy
	static modm_always_inline bool
	clear();

	/// Reset the cursor to (0,0) home position
	/// @return	`true` if operation successful, `false` if controller is busy
	static modm_always_inline bool
	resetCursor();

	// write
	/// Set the address of the cursor
	/// @return	`true` if operation successful, `false` if controller is busy
	static inline bool
	writeAddress(uint8_t address);

	/// Write a command to the controller
	/// @return	`true` if operation successful, `false` if controller is busy
	static inline bool
	writeCommand(uint8_t command);

	/// Write data to the data RAM at the current cursor position
	/// @return	`true` if operation successful, `false` if controller is busy
	static inline bool
	writeRAM(uint8_t data);

	// read
	/// Read the cursor position
	/// @return	`true` if operation successful, `false` if controller is busy
	static modm_always_inline bool
	readAddress(uint8_t &address);

	/// Read the character at the current cursor position
	/// @return	`true` if operation successful, `false` if controller is busy
	static inline bool
	readRAM(uint8_t &data);

	// status
	/// @return `true` if controller is busy with an internal operation, else `false`
	static inline bool
	isBusy();

	static inline bool
	writeCGRAM(uint8_t character, uint8_t *cg);

protected:
	/// unconditionally write data to the controller
	static modm_always_inline void
	write(uint8_t data);

	/// unconditionally read data from the controller
	static modm_always_inline uint8_t
	read();

private:
	// R\W line
	static constexpr bool RW_Write		= modm::Gpio::Low;
	static constexpr bool RW_Read		= modm::Gpio::High;
	// Rs line
	static constexpr bool RS_Command	= modm::Gpio::Low;
	static constexpr bool RS_RAM		= modm::Gpio::High;
	// Enable line
	static constexpr bool E_Disable		= modm::Gpio::Low;
	static constexpr bool E_Enable		= modm::Gpio::High;

	// Masks
	static constexpr uint8_t CGRAM_AddressMask	= 0b00111111;
	static constexpr uint8_t DDRAM_AddressMask	= 0b01111111;
	static constexpr uint8_t BusyFlagMask		= 0b10000000;

	// check correct Bus width
	static_assert(DATA::width == 8 || DATA::width == 4,
			"This driver only works with 4bit or 8bit data bus width!");

	/**
	 *  Bus class for accessing the bus with either 4bit or 8bit width.
	 *
	 * We want the same write read interface for both 4bit and 8bit busses
	 * to get that, we specialize on the Port::width property.
	 * @warning you need to set RW externally, this only wraps the bus access!
	 */
	template<typename Data, typename Enable, uint8_t Width>
	struct Bus;

	/// 4Bit Bus implementation
	template<typename Data, typename Enable>
	struct Bus <Data, Enable, 4>
	{
		/// writes data on the port and toggles the Enable line
		static inline void
		write(uint8_t data);

		/// only writes the high nibble of data
		/// Use this in the initialization, when bus width is not determined yet
		static modm_always_inline void
		writeHighNibble(uint8_t data);

		/// toggles the Enable line and reads the port
		static inline uint8_t
		read();

		/// @see InternalCommand
		static constexpr uint8_t Mode = Set4BitBus;
	};

	/// 8Bit Bus implementation
	template<typename Data, typename Enable>
	struct Bus <Data, Enable, 8>
	{
		/// writes data on the port and toggles the Enable line
		static inline void
		write(uint8_t data);

		/// only writes the high nibble of data
		/// Use this in the initialization, when bus width is not determined yet
		static modm_always_inline void
		writeHighNibble(uint8_t data);

		/// toggles the Enable line and reads the port
		static inline uint8_t
		read();

		/// @see InternalCommand
		static constexpr uint8_t Mode = Set8BitBus;
	};
};

} // namespace modm

#include "hd44780_base_impl.hpp"

#endif // MODM_HD44780_BASE_HPP
