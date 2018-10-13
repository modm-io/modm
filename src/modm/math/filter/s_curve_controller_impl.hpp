/*
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2017, Michael Thies
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_S_CURVE_CONTROLLER_HPP
	#error	"Don't include this file directly, use 's_curve_controller.hpp' instead!"
#endif


#include <modm/math/utils.hpp>

// ----------------------------------------------------------------------------
template<typename T>
modm::SCurveController<T>::Parameter::Parameter(
		const T& targetArea, const T& increment, const T& decreaseFactor, const T& kp,
		const T& speedMaximum, const T& speedMinimum, const T& speedTarget) :
	targetArea(targetArea), increment(increment), decreaseFactor(decreaseFactor), kp(kp),
	speedMaximum(speedMaximum), speedMinimum(speedMinimum),
	speedTarget(speedTarget)
{
}

// ----------------------------------------------------------------------------

template<typename T>
void
modm::SCurveController<T>::setParameter(const Parameter& parameter)
{
	this->parameter = parameter;
	this->additionalDistanceToStop = (parameter.speedTarget * parameter.speedTarget) / parameter.decreaseFactor / 2 ;
}

// ----------------------------------------------------------------------------

template<typename T>
modm::SCurveController<T>::SCurveController(const Parameter& parameter) :
	output(), targetReached(false), parameter(parameter)
{
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
modm::SCurveController<T>::setSpeedMaximum( const T& speed )
{
	this->parameter.speedMaximum = speed;
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
modm::SCurveController<T>::setSpeedMinimim( const T& speed )
{
	this->parameter.speedMinimum = speed;
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
modm::SCurveController<T>::setSpeedTarget( const T& speed )
{
	this->parameter.speedTarget = speed;
	this->additionalDistanceToStop = (speed * speed) / parameter.decreaseFactor / 2;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::SCurveController<T>::isTargetReached() const
{
	return this->targetReached;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::SCurveController<T>::update(T error, const T& speed)
{
	// adjust sign to be always positive
	bool invert = false;
	T currentValue = speed;
	if (error < 0)
	{
		invert = true;
		error = -error;
		currentValue = -currentValue;
	}

	T outputIncrement = currentValue + parameter.increment;
	T outputDecrement;

	targetReached = (error <= parameter.targetArea);
	error += this->additionalDistanceToStop;

	if (error <= parameter.targetArea)
	{
		outputDecrement = error * parameter.kp;
	}
	else {
		outputDecrement = std::sqrt(error * parameter.decreaseFactor * 2);
	}

	output = modm::min(outputIncrement, outputDecrement);
	// TODO smooth breaking if the speedMaximum has changed to a lower value
	output = modm::min(output, parameter.speedMaximum);

	if (output < parameter.speedMinimum) {
		output = parameter.speedMinimum;
	}

	// revert sign
	if (invert) {
		output = -output;
	}
}

// ----------------------------------------------------------------------------
template<typename T>
inline const T&
modm::SCurveController<T>::getValue() const
{
	return this->output;
}
