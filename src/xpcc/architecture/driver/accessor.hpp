/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ACCESSOR_HPP
#define XPCC__ACCESSOR_HPP

#include "accessor/ram.hpp"
#include "accessor/flash.hpp"
#include "accessor/unaligned.hpp"

namespace xpcc
{
	/**
	 * \defgroup	accessor	Accessor classes
	 * \ingroup		architecture
	 * 
	 * \author	Fabian Greif
	 */
	namespace accessor
	{
		/**
		 * \brief	Add volatile modifier to T
		 * 
		 * \ingroup	accessor
		 */
		template<typename T>
		xpcc_always_inline volatile T&
		asVolatile(T& value)
		{
			return (volatile T&) value;
		}
	}
}

#endif // XPCC__ACCESSOR_HPP
