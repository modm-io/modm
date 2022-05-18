/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2013, 2016, Niklas Hauser
 * Copyright (c) 2015, Daniel Krebs
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <concepts>
#include <stdint.h>

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>

#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/**
 * Button masks.
 *
 * Provided for convenience only.
 * Normally it is best to define your own meaningful names for the buttons.
 */
enum ButtonGroupIdentifier
{
	NONE = 0x00,
	BUTTON0 = Bit0,
	BUTTON1 = Bit1,
	BUTTON2 = Bit2,
	BUTTON3 = Bit3,
	BUTTON4 = Bit4,
	BUTTON5 = Bit5,
	BUTTON6 = Bit6,
	BUTTON7 = Bit7,
	ALL = 0xFF,
};

/**
 * @tparam	T	Storage type for Button states. Each button requires one bit.
 *
 * @ingroup	modm_ui_button
 */
template <std::unsigned_integral T = uint8_t>
class ButtonGroup
{
public:
	struct State {
		T value{0};

		bool read(T mask) const {
			return mask & value;
		}

		bool read_and_clear(T mask) {
			mask &= value;
			value ^= mask;
			return mask;
		}
	};

	State pressed;
	State released;
	State repeated;

	/**
	 * @param	repeatmask	only buttons listed here can be used with the methods
	 * 						isRepeated(), isPressedShort() and isPressedLong()
	 * @param	timeout		Button press period (number of update cycles) until begin of repeat
	 * @param	interval	Button press period (number of update cycles) for follow up repeats
	 */
	ButtonGroup(T repeatmask, uint16_t timeout = 50, uint16_t interval = 20)
		: repeatmask(repeatmask), timeout(timeout), interval(interval), repeatCounter(timeout)
	{}

	/// Get the current (debounced) state of a key
	T
	getState(T mask) const {
		atomic::Lock lock;
		return debounced.read(mask);
	}

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
	bool
	isPressed(T mask) {
		atomic::Lock lock;
		return pressed.read_and_clear(mask);
	}

	/// Check if a key has been released
	bool
	isReleased(T mask) {
		atomic::Lock lock;
		return released.read_and_clear(mask);
	}

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
	bool
	isRepeated(T mask) {
		atomic::Lock lock;
		return repeated.read_and_clear(mask);
	}

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
	bool
	isPressedShort(T mask) {
		atomic::Lock lock;

		// get all keys which were pressed but are currently not pressed. This
		// must be a short press then, otherwise the isPressedLong() method
		// would have reseted pressed.
		mask = mask & pressed.value & ~debounced.value;
		pressed.value ^= mask;

		return mask;
	}

	/**
	 * Get buttons which were pressed long
	 *
	 * @warning	Use this function only in combination with
	 *			`isPressedShort()`, otherwise it will not work correctly!
	 * @see	isPressedShort()
	 */
	bool
	isPressedLong(T mask) {
		atomic::Lock lock;

		// get all keys which are long enough pressed so that the repeated
		// variable was set
		mask = mask & repeated.value;
		repeated.value ^= mask;
		mask = mask & pressed.value;
		pressed.value ^= mask;

		return mask;
	}

	/**
	 * @brief			Update internal debounced. Call this function periodically every 5 to 10ms
	 *
	 * @param input		Inverted input signals
	 */
	void
	update(T input) {
		// key changed?
		T i = debounced.value ^ ~input;
		// reset or count ct0
		ct0 = ~(ct0 & i);
		// reset or count ct1
		ct1 = ct0 ^ (ct1 & i);
		// count until roll over?
		i &= ct0 & ct1;

		// then toggle debounced
		debounced.value ^= i;
		// 0->1: key press detected
		pressed.value |= debounced.value & i;
		// 0->1: key release detected
		released.value |= ~debounced.value & i;

		if ((debounced.value & repeatmask) == 0) {
			repeatCounter = timeout;
		}
		if (--repeatCounter == 0) {
			repeatCounter = interval;
			repeated.value |= debounced.value & repeatmask;
		}
	}

private:
	const T repeatmask;
	State debounced;

	const uint16_t timeout;
	const uint16_t interval;
	uint16_t repeatCounter;

	T ct0{0};
	T ct1{0};
};
}