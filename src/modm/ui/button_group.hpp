/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2013, 2016, Niklas Hauser
 * Copyright (c) 2015, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BUTTON_GROUP_HPP
#define MODM_BUTTON_GROUP_HPP

#include <stdint.h>

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>

namespace modm
{
/**
 * @tparam	T
 * 		Container type: `uint8_t` for eight buttons or `uint16_t` for
 * 		up to 16 buttons
 *
 * @ingroup	modm_ui_button
 */
template <typename T = uint8_t>
class ButtonGroup
{
public:
	/**
	 * Button masks.
	 *
	 * Provided for convenience only.
	 * Normally it is best to define your own meaningful names for the buttons.
	 */
	enum Identifier
	{
		NONE = 0x00,
		BUTTON0 = 0x01,
		BUTTON1 = 0x02,
		BUTTON2 = 0x04,
		BUTTON3 = 0x08,
		BUTTON4 = 0x10,
		BUTTON5 = 0x20,
		BUTTON6 = 0x40,
		BUTTON7 = 0x80,
		ALL = 0xFF,
	};

public:
	/**
	 * Constructor
	 *
	 * @param	mask
	 * 		Repeat mask, only buttons listed here can be used with the methods
	 * 		isRepeated(), isPressedShort() and isPressedLong()
	 * @param	timeout
	 * 		Timeout for the repeat operation (number of update cycles)
	 * @param	interval
	 * 		Repeat interval (number of update cycles)
	 */
	ButtonGroup(T mask, uint16_t timeout = 50, uint16_t interval = 20);

	/// Get the current (debounced) state of a key
	T
	getState(T mask) const;

	/// Check if a key has been released
	T
	isReleased(T mask);

	/**
	 * Check if a key has been pressed.
	 * Each pressed key is only reported once.
	 *
	 * @code
	 * modm::ButtonGroup<> buttons;
	 * ...
	 * while (true) {
	 *		...
	 * 	if (buttons.isPressed(modm::ButtonGroup::BUTTON0)) {
	 * 		// do something
	 * 	}
	 * }
	 * @endcode
	 */
	T
	isPressed(T mask);

	/**
	 * Check if a key has been pressed long enough such that the key repeat
	 * functionality kicks in.
	 *
	 * After a small setup delay the key is reported being pressed
	 * subsequent calls to this function. This simulates the user
	 * repeatedly pressing and releasing the key.
	 *
	 * @code
	 * modm::ButtonGroup<> buttons(modm::ButtonGroup::BUTTON0);
	 * ...
	 * while (true) {
	 *		...
	 * 	if (buttons.isPressed(modm::ButtonGroup::BUTTON0) ||
	 *		buttons.isRepeated(modm::ButtonGroup::BUTTON0))
	 * 	{
	 * 		// do something
	 * 	}
	 * }
	 * @endcode
	 *
	 * @see	isPressed()
	 */
	T
	isRepeated(T mask);

	/**
	 * Get buttons which were pressed short.
	 *
	 * @code
	 * modm::ButtonGroup<> buttons(modm::ButtonGroup::BUTTON0);
	 * ...
	 * while (true) {
	 * 	...
	 * 	if (buttons.isPressedShort(modm::ButtonGroup::BUTTON0)) {
	 * 		// do something
	 * 	}
	 * 	if (buttons.isPressedLong(modm::ButtonGroup::BUTTON0)) {
	 * 		// do something different
	 * 	}
	 * }
	 * @endcode
	 *
	 * @warning	Use this function only in combination with
	 *			`isPressedLong()`, otherwise it will not work correctly!
	 *
	 * @see	isPressedLong()
	 */
	T
	isPressedShort(T mask);

	/**
	 * Get buttons which were pressed long
	 *
	 * @warning	Use this function only in combination with
	 *			`isPressedShort()`, otherwise it will not work correctly!
	 * @see	isPressedShort()
	 */
	T
	isPressedLong(T mask);

	/**
	 * Update internal state.
	 *
	 * Call this function periodically every 5 to 10ms
	 *
	 * @param	input
	 * 		input signals
	 */
	void
	update(T input);

protected:
	const uint16_t timeout;
	const uint16_t interval;
	const T repeatMask;
	uint16_t repeatCounter;

	T state;		///< debounced and inverted key state:
					///< bit = 1: key pressed
	T pressState;
	T releaseState;
	T repeatState;

private:
	T ct0;
	T ct1;
};
}

#include "button_group_impl.hpp"

#endif // MODM_BUTTON_GROUP_HPP
