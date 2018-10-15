/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_S_CURVE_GENERATOR_HPP
#define MODM_S_CURVE_GENERATOR_HPP

namespace modm
{
	/**
	 * \brief	Trapezoidal S-Curve Trajectory
	 *
	 * open-loop control
	 *
	 * \todo	documentation
	 * \todo	implementation
	 * \ingroup	modm_math_filter
	 */
	template<typename T>
	class SCurveGenerator
	{
	public:
		SCurveGenerator(const T& initialValue = T());

		inline void
		setTarget(const T& target);

		void
		update();

		inline const T&
		getValue() const
		{
			return value;
		}

		inline bool
		isTargetReached() const;

	private:
		T target;
		T value;
		bool targetReached;
	};
}

#include "s_curve_generator_impl.hpp"

#endif // MODM_S_CURVE_GENERATOR_HPP
