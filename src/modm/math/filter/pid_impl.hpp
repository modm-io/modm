/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PID_IMPL_HPP
#define MODM_PID_IMPL_HPP

template<typename T, unsigned int ScaleFactor>
modm::Pid<T, ScaleFactor>::Parameter::Parameter(
		const float& kp, const float& ki, const float& kd,
		const T& maxErrorSum, const T& maxOutput) :
	kp(static_cast<T>(kp * ScaleFactor)),
	ki(static_cast<T>(ki * ScaleFactor)),
	kd(static_cast<T>(kd * ScaleFactor)),
	maxErrorSum(static_cast<T>(maxErrorSum * ScaleFactor)),
	maxOutput(maxOutput)
{
}

// -----------------------------------------------------------------------------
template<typename T, unsigned int ScaleFactor>
modm::Pid<T, ScaleFactor>::Pid(
		const float& kp, const float& ki, const float& kd,
		const T& maxErrorSum, const T& maxOutput) :
	parameter(kp, ki, kd, maxErrorSum, maxOutput)
{
	this->reset();
}

// -----------------------------------------------------------------------------
template<typename T, unsigned int ScaleFactor>
modm::Pid<T, ScaleFactor>::Pid(
		Parameter& parameter) :
	parameter(parameter)
{
	this->reset();
}

template<typename T, unsigned int ScaleFactor>
void
modm::Pid<T, ScaleFactor>::reset()
{
	this->errorSum = 0;
	this->lastError = 0;
	this->output = 0;
}

template<typename T, unsigned int ScaleFactor>
void
modm::Pid<T, ScaleFactor>::setParameter(const Parameter& parameter)
{
	this->parameter = parameter;
}

template<typename T, unsigned int ScaleFactor>
void
modm::Pid<T, ScaleFactor>::update(const T& input, bool externalLimitation)
{
	bool limitation = externalLimitation;

	T tempErrorSum = errorSum + input;
	if (tempErrorSum > this->parameter.maxErrorSum) {
		tempErrorSum = this->parameter.maxErrorSum;
	}
	else if (tempErrorSum < -this->parameter.maxErrorSum) {
		tempErrorSum = -this->parameter.maxErrorSum;
	}

	WideType tmp = 0;
	tmp += static_cast<WideType>(this->parameter.kp) * input;
	tmp += static_cast<WideType>(this->parameter.ki) * (tempErrorSum);
	tmp += static_cast<WideType>(this->parameter.kd) * (input - this->lastError);

	tmp = tmp / ScaleFactor;

	if (tmp > this->parameter.maxOutput) {
		this->output = this->parameter.maxOutput;
		limitation = true;
	}
	else if (tmp < -this->parameter.maxOutput) {
		this->output = -this->parameter.maxOutput;
		limitation = true;
	}
	else {
		this->output = tmp;
	}

	// If an external limitation (saturation somewhere in the control loop) is
	// applied the error sum will only be decremented, never incremented.
	// This is done to help the system to leave the saturated state.
	if (not limitation or (std::abs(tempErrorSum) < std::abs(this->errorSum)))
	{
		this->errorSum = tempErrorSum;
	}

	this->lastError = input;
}

#endif // MODM_PID_IMPL_HPP
