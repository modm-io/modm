/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FILTER_RAMP_HPP
	#error	"Don't include this file directly, use 'ramp.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::filter::Ramp<T>::Ramp(const T& increment, const T& decrement, const T& initialValue) :
	target(initialValue),
	value(initialValue),
	targetReached(true),
	increment(increment),
	decrement(decrement)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::filter::Ramp<T>::setTarget(const T& target)
{
	this->target = target;
	this->targetReached = false;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::filter::Ramp<T>::update()
{
	if (target > value)
	{
		T variation = target - value;
		if (variation > increment) {
			value += increment;
		}
		else {
			value = target;
			targetReached = true;
		}
	}
	else
	{
		T variation = value - target;
		if (variation > decrement) {
			value -= decrement;
		}
		else {
			value = target;
			targetReached = true;
		}
	}
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::filter::Ramp<T>::reset(const T& value)
{
	this->value = value;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
modm::filter::Ramp<T>::getValue() const
{
	return this->value;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::filter::Ramp<T>::isTargetReached() const
{
	return targetReached;
}
