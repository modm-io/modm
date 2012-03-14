// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PID_IMPL_HPP
#define XPCC__PID_IMPL_HPP

template<typename T, unsigned int ScaleFactor>
xpcc::Pid<T, ScaleFactor>::Parameter::Parameter(
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
xpcc::Pid<T, ScaleFactor>::Pid(
		const float& kp, const float& ki, const float& kd,
		const T& maxErrorSum, const T& maxOutput) :
	parameter(kp, ki, kd, maxErrorSum, maxOutput)
{
	this->reset();
}

// -----------------------------------------------------------------------------
template<typename T, unsigned int ScaleFactor>
xpcc::Pid<T, ScaleFactor>::Pid(
		Parameter& parameter) :
	parameter(parameter)
{
	this->reset();
}

template<typename T, unsigned int ScaleFactor>
void
xpcc::Pid<T, ScaleFactor>::reset()
{
	this->errorSum = 0;
	this->lastError = 0;
	this->output = 0;
}

template<typename T, unsigned int ScaleFactor>
void
xpcc::Pid<T, ScaleFactor>::setParameter(const Parameter& parameter)
{
	this->parameter = parameter;
}

template<typename T, unsigned int ScaleFactor>
void
xpcc::Pid<T, ScaleFactor>::update(const T& input, bool externalLimitation)
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

#endif // XPCC__PID_IMPL_HPP
