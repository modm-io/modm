/*
 * Copyright (c) 2009, Thorsten Lajewski
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

#ifndef	MODM_INTERPOLATION_LINEAR_HPP
   #error "Don't include this file directly. Use 'modm/math/interpolation/linear.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
modm::interpolation::Linear<T, Accessor>::Linear(
		Accessor<T> supportingPoints, uint8_t numberOfPoints) :
	supportingPoints(supportingPoints), numberOfPoints(numberOfPoints)
{
}

// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
typename modm::interpolation::Linear<T, Accessor>::OutputType
modm::interpolation::Linear<T, Accessor>::interpolate(const InputType& value) const
{
	T current(this->supportingPoints[0]);

	if (value <= current.getFirst()) {
		return current.getSecond();
	}

	T last(current);
	uint8_t i;
	for (i = 1; i < this->numberOfPoints; ++i)
	{
		current = this->supportingPoints[i];

		if (value <= current.getFirst())
		{
			InputType x1_in = last.getFirst();
			InputType x2_in = current.getFirst();

			OutputType x1_out = last.getSecond();
			OutputType x2_out = current.getSecond();

			InputType a = value - x1_in;		// >0
			WideType b = static_cast<OutputSignedType>(x2_out) -
						 static_cast<OutputSignedType>(x1_out);
			InputType c = x2_in - x1_in;		// >0

			return static_cast<OutputType>(((a * b) / c) + x1_out);
		}

		last = current;
	}

	return current.getSecond();
}
