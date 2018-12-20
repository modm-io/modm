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

#ifndef MODM_PID_HPP
#define MODM_PID_HPP

#include <cstdlib>
#include <cmath>
#include <stdint.h>

#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm
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
	 * Example for a motor speed control with a 10-bit PWM output.
	 * \code
	 * Pid<int16_t, 10> pid(0.4, 0.5, 0, 200, 512);
	 *
	 * ...
	 *
	 * v_target = ... // setpoint
	 * v_input  = ... // input value
	 *
	 * pid.update(v_target - v_input);
	 * pwm = pid.getValue();
	 * \endcode
	 *
	 * \todo	use the faster avr::mul and avr::mac functions
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_math_filter
	 */
	template<typename T, unsigned int ScaleFactor = 1>
	class Pid
	{
		typedef modm::WideType<T> WideType;

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

		/**
		 * \brief	Get last error
		 *
		 * This function is provided for debugging purposes only.
		 *
		 * The differential term is calculated via:
		 * \code
		 * Kd * (input - this->lastError);
		 * \endcode
		 */
		inline const T&
		getLastError() const
		{
			return this->lastError;
		}

		/**
		 * \brief	Get integrated error
		 *
		 * This function is provided for debugging purposes only.
		 */
		inline const T&
		getErrorSum() const
		{
			return this->errorSum;
		}

	private:
		Parameter parameter;

		T errorSum;
		T lastError;
		T output;
	};
}

#include "pid_impl.hpp"

#endif // MODM_PID_HPP
