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

#ifndef XPCC__PID_HPP
#define XPCC__PID_HPP

#include <cstdlib>
#include <cmath>

#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{
	/**
	 * \brief	A proportional-integral-derivative controller (PID controller)
	 *
	 * The PID controller is one of the basic controlling algorithm.
	 *
	 * The \p maxErrorSum is to be used, to limit the internal integrator and so
	 * provide an anti wind up.
	 *
	 * With the template parameter \c ScaleFactor this class provides an
	 * fix point capability with integer types.
	 * 
	 * \todo	use the faster avr::mul and avr::mac functions
	 * 
	 * \author	Fabian Greif
	 * \ingroup	filter
	 */
	template<typename T, unsigned int ScaleFactor = 1>
	class Pid
	{
		typedef typename ArithmeticTraits<T>::WideType WideType;
		
	public:
		typedef T ValueType;

		/**
		 * \brief	Parameter for a PID calculation
		 */
		struct Parameter
		{
			/// \todo	description why we use float here!
			/// \todo	calculate maxErrorSum from the parameters
			Parameter(const float& kp = 0, const float& ki = 0, const float& kd = 0,
					  const T& maxErrorSum = 0, const T& maxOutput = 0);
			
			inline void
			setKp(float kp) {
				this->kp = static_cast<T>(kp * ScaleFactor);
			}
			
			inline void
			setKi(float ki) {
				this->ki = static_cast<T>(ki * ScaleFactor);
			}
			
			inline void
			setKd(float kd) {
				this->kd = static_cast<T>(kd * ScaleFactor);
			}
			
			inline void
			setMaxErrorSum(float maxErrorSum) {
				this->maxErrorSum = static_cast<T>(maxErrorSum * ScaleFactor);
			}
			
		private:
			T kp;		///< Proportional gain multiplied with ScaleFactor
			T ki;		///< Integral gain multiplied with ScaleFactor
			T kd;		///< Differential gain multiplied with ScaleFactor
			
			T maxErrorSum;	///< integral will be limited to this value
			T maxOutput;	///< output will be limited to this value
			
			friend class Pid;
		};
		
	public:
		/**
		 * \param	kp	proportional gain
		 * \param	ki	integral gain
		 * \param	kd	differential gain
		 * \param	maxErrorSum	integral will be limited to this value
		 * \param	maxOutput	output will be limited to this value
		 **/
		Pid(const float& kp = 0, const float& ki = 0, const float& kd = 0,
			const T& maxErrorSum = 0, const T& maxOutput = 0);
		
		/**
		 * \param	parameter	list of parameters to the controller
		 **/
		Pid(Parameter& parameter);
		
		/**
		 * Reset the parameters of the controller.
		 *
		 * \param	parameter	list of parameters to the controller
		 **/
		void
		setParameter(const Parameter& parameter);
		
		/**
		 * \brief	Reset all values
		 */
		void
		reset();
		
		/**
		 * \brief	Calculate a new output value
		 * 
		 * \param	input				Error
		 * \param	externalLimitation	If true an external limitation is applied,
		 * 								this disables integral summation.
		 */
		void
		update(const T& input, bool externalLimitation = false);
		
		/**
		 * \brief	Returns the calculated actuating variable.
		 */
		inline const T&
		getValue() const
		{
			return output;
		}

	private:
		Parameter parameter;
		
		T errorSum;
		T lastError;
		T output;
	};
}

#include "pid_impl.hpp"

#endif // XPCC__PID_HPP
