/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2013, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/math.hpp>

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	while (1)
	{
		xpcc::Vector<float, 2> v(1.0f, 2.0f);
		v.getLength();

		xpcc::Quaternion<float> q;
		q.getLength();
	}
}
