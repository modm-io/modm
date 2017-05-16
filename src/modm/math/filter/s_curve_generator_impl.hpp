/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_S_CURVE_GENERATOR_HPP
	#error	"Don't include this file directly, use 's_curve_generator.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::SCurveGenerator<T>::SCurveGenerator(const T& initialValue) :
	target(), value(initialValue), targetReached(false)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::SCurveGenerator<T>::setTarget(const T& target)
{
	this->target = target;
	targetReached = false;
}

template<typename T>
void
modm::SCurveGenerator<T>::update()
{
}

template<typename T>
bool
modm::SCurveGenerator<T>::isTargetReached() const
{
	return targetReached;
}
