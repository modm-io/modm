/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BUTTON_HPP
	#error	"Don't include this file directly, use 'button.hpp' instead!"
#endif

template <typename T> uint8_t xpcc::Button<T>::state;

template <typename T>
void
xpcc::Button<T>::update()
{
	uint8_t i;
	if (T::read()) {
		i = 0;
	}
	else {
		i = 1;
	}
	
	i = i ^ state;
	i = i << 1 | 0xc0;
	
	// FIXME
	if (i == 0xef)
	{
		state ^= 0x01;
	}
	else
	{
		
	}
}

template <typename T>
bool
xpcc::Button<T>::getState()
{
	return (state & 0x01);
}

template <typename T>
bool
xpcc::Button<T>::isPressed()
{
	return false;
}
