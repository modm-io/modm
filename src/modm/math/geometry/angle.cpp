/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/misc.hpp>

#include "angle.hpp"

// ----------------------------------------------------------------------------
float
modm::Angle::normalize(float angle)
{
	if (isPositive(angle))
	{
		while (angle > std::numbers::pi_v<float>) {
			angle -= 2 * std::numbers::pi_v<float>;
		}
	}
	else {
		while (angle < -std::numbers::pi_v<float>) {
			angle += 2 * std::numbers::pi_v<float>;
		}
	}

	return angle;
}

// ----------------------------------------------------------------------------
float
modm::Angle::reverse(float angle)
{
	if (isPositive(angle))
	{
		angle -= std::numbers::pi_v<float>;
	}
	else {
		angle += std::numbers::pi_v<float>;
	}

	return angle;
}

// ----------------------------------------------------------------------------
float
modm::Angle::perpendicular(float angle, const bool cw)
{
	if (cw)
	{
		angle = modm::Angle::normalize(angle - std::numbers::pi_v<float>/2);
	}
	else {
		angle = modm::Angle::normalize(angle + std::numbers::pi_v<float>/2);
	}

	return angle;
}
