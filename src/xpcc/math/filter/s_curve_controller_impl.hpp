// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *	   notice, this list of conditions and the following disclaimer in the
 *	   documentation and/or other materials provided with the distribution.
 *	 * Neither the name of the Roboterclub Aachen e.V. nor the
 *	   names of its contributors may be used to endorse or promote products
 *	   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__S_CURVE_CONTROLLER_HPP
	#error	"Don't include this file directly, use 's_curve_controller.hpp' instead!"
#endif

#include <xpcc/architecture/platform.hpp>

// ----------------------------------------------------------------------------
template<typename T>
xpcc::SCurveController<T>::Parameter::Parameter(
		const T& targetArea, const T& increment, const T& decreaseFactor, const T& kp, 
		const T& secondaryMaximum, const T& secondaryMinimum, const T& secondaryTarget) :
	targetArea(targetArea), increment(increment), decreaseFactor(decreaseFactor), kp(kp),
	secondaryMaximum(secondaryMaximum), secondaryMinimum(secondaryMinimum),
	secondaryTarget(secondaryTarget)
{
}			

// ----------------------------------------------------------------------------

template<typename T>
xpcc::SCurveController<T>::SCurveController(const Parameter& parameter) :
	output(), targetReached(false), parameter(parameter)
{
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
xpcc::SCurveController<T>::setSecondaryMaximum( const T& secondary )
{
	this->parameter.secondaryMaximum = secondary;
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
xpcc::SCurveController<T>::setSecondaryMinimim( const T& secondary )
{
	this->parameter.secondaryMinimum = secondary;
}

// ----------------------------------------------------------------------------
template<typename T>
inline void
xpcc::SCurveController<T>::setSecondaryTarget( const T& secondary )
{
	this->parameter.secondaryTarget = secondary;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::SCurveController<T>::isTargetReached() const
{
	return this->targetReached;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::SCurveController<T>::update(T error, const T& secondary)
{
	// adjust sign to be always positive
	bool invert = false;
	T currentValue = secondary;
	if (error < 0)
	{
		invert = true;
		error = -error;
		currentValue = -currentValue;
	}
	
	T outputIncrement = currentValue + parameter.increment;
	T outputDecrement;
	if (error <= parameter.targetArea)
	{
		targetReached = true;
		outputDecrement = error * parameter.kp;
	}
	else {
		targetReached = false;
		outputDecrement = std::sqrt((error) *
			parameter.decreaseFactor * 2) + parameter.secondaryTarget;
	}
	
	output = std::min(outputIncrement, outputDecrement);
	// TODO smoth breaking if the secondaryMaximum has changed to a lower value
	output = std::min(output, parameter.secondaryMaximum);
	
	if (output < parameter.secondaryMinimum) {
		output = parameter.secondaryMinimum;
	}
	
	// revert sign
	if (invert) {
		output = -output;
	}
}

// ----------------------------------------------------------------------------
template<typename T>
inline const T&
xpcc::SCurveController<T>::getValue() const
{
	return this->output;
}
