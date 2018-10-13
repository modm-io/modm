/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Kevin Läufer
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

namespace modm
{
	/// @ingroup modm_math_filter
	namespace filter
	{
	}
}

#include "filter/debounce.hpp"
#include "filter/fir.hpp"
#include "filter/median.hpp"
#include "filter/moving_average.hpp"
#include "filter/pid.hpp"
#include "filter/ramp.hpp"
#include "filter/s_curve_controller.hpp"
#include "filter/s_curve_generator.hpp"
