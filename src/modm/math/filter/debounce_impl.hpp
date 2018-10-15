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

#ifndef MODM_FILTER_DEBOUNCE_HPP
	#error	"Don't include this file directly, use 'debounce.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::filter::Debounce<T>::Debounce(const T& maxValue, const T& lowerBound, const T& upperBound) :
	maxValue(maxValue),
	sum(maxValue / 2),
	lowerBound(lowerBound),
	upperBound((upperBound != 0) ? upperBound : maxValue),
	state(false)
{
}

template<typename T>
void
modm::filter::Debounce<T>::update(bool input)
{
	if (input && (sum < maxValue)) {
		sum++;
	}
	else if (!input && (sum > 0)) {
		sum--;
	}

	if (state && (sum <= lowerBound)) {
		state = false;
	}
	else if (!state && (sum >= upperBound)) {
		state = true;
	}
}

template<typename T>
bool
modm::filter::Debounce<T>::getValue() const
{
	return state;
}

template<typename T>
void
modm::filter::Debounce<T>::reset(const bool state)
{
	if (state) {
		sum = maxValue;
		this->state = true;
	}
	else {
		sum = 0;
		this->state = false;
	}
}
