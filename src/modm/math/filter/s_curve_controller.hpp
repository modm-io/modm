/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
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
#define MODM_S_CURVE_CONTROLLER_HPP

#include <cmath>

namespace modm
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
	 * \ingroup	modm_math_filter
	 */
	template<typename T>
	class SCurveController
	{
	public:
		/**
		 * \todo	some helper programs/formula to choose the right
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
		/// distance to add to the current error for speed calculation. This is automatically calculated from
		/// speedtarget parameter.
		T additionalDistanceToStop;
	};
}

#include "s_curve_controller_impl.hpp"

#endif // MODM_S_CURVE_CONTROLLER_HPP
