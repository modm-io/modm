/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BUTTON_HPP
#define	XPCC__BUTTON_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Simple Button
	 * 
	 * \todo	Implementation is incomplete
	 * 
	 * \ingroup	ui
	 * \author	Fabian Greif
	 */
	template <typename PIN>
	class Button
	{
	public:
		static bool
		getState();
		
		static bool
		isPressed();
		
		static bool
		isReleased();
		
		
		static void
		update();
		
	private:
		static uint8_t state;
	};
}

#include "button_impl.hpp"

#endif	// XPCC__BUTTON_HPP
