/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ALGORITHM_HPP
#define XPCC__ALGORITHM_HPP

namespace xpcc
{
	template<class InputIterator, class T>
	InputIterator
	find (InputIterator first, InputIterator last, const T& value)
	{
		for ( ; first != last; ++first) {
			if (*first == value) {
				break;
			}
		}
		return first;
	}
}

#endif	// XPCC__ALGORITHM_HPP
