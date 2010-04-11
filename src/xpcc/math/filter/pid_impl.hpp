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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PID_IMPL_HPP
#define XPCC__PID_IMPL_HPP

template<typename T, unsigned int ScaleFactor>
xpcc::Pid<T, ScaleFactor>::Parameter::Parameter(
		const float& kp, const float& ki, const float& kd,
		const T& maxErrorSum, const T& maxOutput) :
	kp(kp * ScaleFactor), ki(ki * ScaleFactor), kd(kd * ScaleFactor),
	maxErrorSum(maxErrorSum), maxOutput(maxOutput)
{
}

// -----------------------------------------------------------------------------
template<typename T, unsigned int ScaleFactor>
xpcc::Pid<T, ScaleFactor>::Pid(
		const float& kp, const float& ki, const float& kd,
		const T& maxErrorSum, const T& maxOutput,
		FeedforwardFunction feedforward ) :
	parameter( kp, ki, kd, maxErrorSum, maxOutput),
	feedforward(feedforward)
{
	this->reset();
}

// -----------------------------------------------------------------------------
template<typename T, unsigned int ScaleFactor>
xpcc::Pid<T, ScaleFactor>::Pid(
		Parameter& parameter,
		FeedforwardFunction feedforward) :
	parameter(parameter), feedforward(feedforward)
{
	this->reset();
}

template<typename T, unsigned int ScaleFactor>
void
xpcc::Pid<T, ScaleFactor>::reset()
{
//	this->target = 0;
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
xpcc::Pid<T, ScaleFactor>::update(const T& error)
{
//	T error = this->target - input;
	
	this->errorSum += error;
	if (this->errorSum > this->parameter.maxErrorSum) {
		this->errorSum = this->parameter.maxErrorSum;
	}
	else if (this->errorSum < -this->parameter.maxErrorSum) {
		this->errorSum = -this->parameter.maxErrorSum;
	}
	
	DoubleType tmp = 0;
//	tmp  = this->feedforward(this->target); // eigentlich 
	tmp += static_cast<DoubleType>(this->parameter.kp) * error;
	tmp += static_cast<DoubleType>(this->parameter.ki) * (this->errorSum);
	tmp += static_cast<DoubleType>(this->parameter.kd) * (error - this->lastError);
	
	tmp = tmp / ScaleFactor;
	
	if (tmp > this->parameter.maxOutput) {
		this->output = this->parameter.maxOutput;
	}
	else if (tmp < -this->parameter.maxOutput) {
		this->output = -this->parameter.maxOutput;
	}
	else {
		this->output = tmp;
	}
	
	this->lastError = error;
}

#endif // XPCC__PID_IMPL_HPP
