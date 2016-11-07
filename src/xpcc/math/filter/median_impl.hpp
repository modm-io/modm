/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FILTER__MEDIAN_HPP
	#error	"Don't include this file directly, use 'median.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
#define XPCC_MEDIAN__SORT(a,b) do { if (a > b) { XPCC_MEDIAN__SWAP(a, b); } } while (0);
#define XPCC_MEDIAN__SWAP(a,b) do { T temp = a; a = b; b = temp; } while (0);

#include "median_3_impl.hpp"
#include "median_5_impl.hpp"
#include "median_7_impl.hpp"
#include "median_9_impl.hpp"

#undef XPCC_MEDIAN__SORT
#undef XPCC_MEDIAN__SWAP

// ----------------------------------------------------------------------------
// TODO General implementation
