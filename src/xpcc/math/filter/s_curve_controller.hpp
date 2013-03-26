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

#ifndef XPCC__S_CURVE_CONTROLLER_HPP
#define XPCC__S_CURVE_CONTROLLER_HPP

#include <algorithm>
#include <cmath>

namespace xpcc
{
	/**
	 * \brief	S-Curve trajectory generation with feedback
	 * 
	 * closed-loop
	 * 
	 * \code
	 * SCurveController<int16_t> controller;
	 * 
	 * controller.update(positionTarget - currentPosition, currentSpeed); 
	 * \endcode
	 * 
	 * \todo	documentation
	 * 
	 * \author	Fabian Greif
	 * \author	Georgi Grinshpun
	 * 
	 * \ingroup	filter
	 */
	template<typename T>
	class SCurveController
	{
	public:
		/**
		 * \todo	some helper programs/formular to choose the right
		 * 			parameter values (e.g. the java tool from the last year)
		 * 
		 * \param	increment	=> increment per timestep
		 * \param	decreaseFactor => decrease per second
		 */
		struct Parameter
		{
			Parameter(const T& targetArea = T(),
					const T& increment = T(),
					const T& decreaseFactor = T(),
					const T& kp = T(),
					const T& speedMaximum = T(),
					const T& speedMinimum = T(),
					const T& speedTarget = T());
			
			T targetArea;
			T increment;
			T decreaseFactor;
			T kp;
			T speedMaximum;
			T speedMinimum;
			T speedTarget;
		};
		
	public:
		SCurveController(const Parameter& parameter);

		/**
		 * Reset the parameters of the controller.
		 *
		 * \param	parameter list of the controller
		 **/
		void
		setParameter(const Parameter& parameter);
		
		inline void
		setSpeedMaximum(const T& speed);

		inline void
		setSpeedMinimim(const T& speed);

		inline void
		setSpeedTarget(const T& speed);

		inline bool
		isTargetReached() const;
		
		void
		update(T error, const T& speed);
		
		/// setpoint output for the speed value
		inline const T&
		getValue() const;
		
	private:
		T output;
		bool targetReached;
		
		Parameter parameter;
	};
}

#include "s_curve_controller_impl.hpp"

#endif // XPCC__S_CURVE_CONTROLLER_HPP
