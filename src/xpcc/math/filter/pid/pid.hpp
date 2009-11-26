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
 * $Id: pid.hpp 76 2009-10-14 23:29:28Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PID_HPP
#define XPCC__PID_HPP

#include "../../../utils/arithmetic_traits.hpp"

namespace xpcc
{
	template<typename T>
	T
	feedforwardDummy(const T& in) {
		return in;
	}
	
	/// \ingroup	filter
	/// \brief	A proportional-integral-derivative controller (PID controller)
	///
	/// \todo	check implementation
	template<typename T, unsigned int ScaleFactor = 1>
	class Pid
	{
		typedef typename ArithmeticTraits<T>::DoubleType T_DOUBLE;
		typedef T (* FeedforwardFunction)(const T&);
	
	public:
		struct Parameter
		{
			Parameter(const T& kp = 0, const T& ki = 0, const T& kd = 0,
					  const T& maxErrorSum = 0, const T& maxOutput = 0) :
				kp(kp), ki(ki), kd(kd),
				maxErrorSum(maxErrorSum), maxOutput(maxOutput) {
			}
			
			T kp;
			T ki;
			T kd;
			
			T maxErrorSum;
			T maxOutput;
		};
		
	public:
		Pid(Parameter& parameter,
			FeedforwardFunction feedforward = feedforwardDummy<T>);
		
		void
		setParameter(const Parameter& param);
		
		/// \brief	Reset all values
		void
		reset();
		
		/// \brief	Set a new target value
		void
		setTarget(const T& value)
		{
			target = value;
		}
		
		/// \brief	Calculate a new output value
		void
		update(const T& input);
		
		const T&
		getValue() const
		{
			return output;
		}
	
	private:
		Parameter parameter;
		FeedforwardFunction feedforward;
		
		T target;
		T errorSum;
		T lastError;
		T output;
	};
}

#include "pid_impl.hpp"

#endif // XPCC__PID_HPP
