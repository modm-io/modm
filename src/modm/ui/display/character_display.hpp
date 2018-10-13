/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CHARACTER_DISPLAY_HPP
#define MODM_CHARACTER_DISPLAY_HPP

#include <stdint.h>
#include <modm/io/iostream.hpp>

namespace modm
{

/**
 * Base class for alpha-numeric LCDs (liquid crystal display)
 *
 * About this implementation:
 *
 * In contrast to other drivers which use static methods,
 * this class is directly derived from `modm::IODevice` with all the virtual
 * methods because it will be most likely used to create a IOStream
 * which then handles the output to the display.
 *
 * Therefore creating this class with static methods and using a wrapper
 * class to create a `modm::IODevice` seems unnecessary complicated.
 *
 * @author	Fabian Greif
 * @ingroup	modm_ui_display
 */
class CharacterDisplay : public IOStream
{
public:
	enum
	Command
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
	};

public:
	/// Constructor
	CharacterDisplay(uint8_t width, uint8_t height);

	/// Initialize the display
	void
	initialize();

	/**
	 * Write a character
	 *
	 * This method provides an automatic wrap-round if the output reaches
	 * the end of the current line or a newline character is detected.
	 *
	 * Use writeRaw() if this behavior is not wanted.
	 */
	void
	write(char c);

	/**
	 * Write a raw character at cursor position
	 *
	 * Unlike write() no further processing will occur.
	 *
	 * @see	write()
	 */
	virtual void
	writeRaw(char c) = 0;

	/**
	 * Excute a command.
	 *
	 * @see Command
	 * @param	command		Command to execute
	 */
	void
	execute(Command command);

	/**
	 * Set cursor to specified position
	 *
	 * @param	column	horizontal position
	 * @param	line	vertical position
	 */
	virtual void
	setCursor(uint8_t column, uint8_t line) = 0;

	/// clear the entire display and reset the cursor
	void
	clear();

protected:
	// Interface class for the IOStream
	class Writer : public IODevice
	{
	public:
		Writer(CharacterDisplay *parent) :
			parent(parent)
		{
		}

		/// Draw a single character
		virtual void
		write(char c);

		using IODevice::write;

		/// unused
		virtual void
		flush();

		/// unused, returns always \c false
		virtual bool
		read(char& c);

	private:
		CharacterDisplay *parent;
	};

protected:
	Writer writer;

	uint8_t lineWidth;
	uint8_t lineCount;

	uint8_t column;
	uint8_t line;
};

} // namespace modm

#endif // MODM_CHARACTER_DISPLAY_HPP
