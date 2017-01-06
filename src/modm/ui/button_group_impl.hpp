/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BUTTON_GROUP_HPP
	#error	"Don't include this file directly, use 'button_group.hpp' instead!"
#endif

template <typename T>
modm::ButtonGroup<T>::ButtonGroup(T mask, uint16_t inTimeout, uint16_t interval) :
	timeout(inTimeout), interval(interval), repeatMask(mask), repeatCounter(inTimeout),
	state(), pressState(), releaseState(), repeatState(), ct0(), ct1()
{
}

// ----------------------------------------------------------------------------
template <typename T>
T
modm::ButtonGroup<T>::getState(T mask) const
{
	atomic::Lock lock;

	return mask & state;
}

// ----------------------------------------------------------------------------
template <typename T>
T
modm::ButtonGroup<T>::isReleased(T mask)
{
	atomic::Lock lock;

	mask &= releaseState;
	// clear releaseState bits
	releaseState ^= mask;

	return mask;
}

// ----------------------------------------------------------------------------
template <typename T>
T
modm::ButtonGroup<T>::isPressed(T mask)
{
	atomic::Lock lock;

	mask &= pressState;
	// clear pressState bits
	pressState ^= mask;

	return mask;
}

template <typename T>
T
modm::ButtonGroup<T>::isRepeated(T mask)
{
	atomic::Lock lock;

	mask &= repeatState;
	// clear repeatState bits
	repeatState ^= mask;

	return mask;
}

// ----------------------------------------------------------------------------
template <typename T>
T
modm::ButtonGroup<T>::isPressedShort(T mask)
{
	atomic::Lock lock;

	// get all keys which were pressed but are currently not pressed. This
	// must be a short press then, otherwise the isPressedLong() method
	// would have reseted pressState.
	mask = mask & pressState & ~state;
	pressState ^= mask;

	return mask;
}

template <typename T>
T
modm::ButtonGroup<T>::isPressedLong(T mask)
{
	atomic::Lock lock;

	// get all keys which are long enough pressState so that the repeatState
	// variable was set
	mask = mask & repeatState;
	repeatState ^= mask;
	mask = mask & pressState;
	pressState ^= mask;

	return mask;
}

// ----------------------------------------------------------------------------
template <typename T>
void
modm::ButtonGroup<T>::update(T input)
{
	// key changed?
	T i = state ^ ~input;
	// reset or count ct0
	ct0 = ~(ct0 & i);
	// reset or count ct1
	ct1 = ct0 ^ (ct1 & i);
	// count until roll over?
	i &= ct0 & ct1;

	// then toggle debounced state
	state ^= i;
	// 0->1: key press detected
	pressState |= state & i;
	// 0->1: key release detected
	releaseState |= ~state & i;

	if ((state & repeatMask) == 0) {
		repeatCounter = timeout;
	}
	if (--repeatCounter == 0) {
		repeatCounter = interval;
		repeatState |= state & repeatMask;
	}
}
