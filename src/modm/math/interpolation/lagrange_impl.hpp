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

#ifndef	MODM_INTERPOLATION_LAGRANGE_HPP
   #error "Don't include this file directly. Use 'modm/math/interpolation/lagrange.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
modm::interpolation::Lagrange<T, Accessor>::Lagrange(
		Accessor<T> supportingPoints, uint8_t numberOfPoints) :
	supportingPoints(supportingPoints), numberOfPoints(numberOfPoints)
{
}

// ----------------------------------------------------------------------------

template <typename T,
		  template <typename> class Accessor>
typename modm::interpolation::Lagrange<T, Accessor>::OutputType
modm::interpolation::Lagrange<T, Accessor>::interpolate(const InputType& value) const
{
	OutputType ret = 0;
	for (uint8_t i = 0; i < this->numberOfPoints; ++i)
	{
		OutputType li = 1;
		for (uint8_t j = 0; j < this->numberOfPoints; ++j)
		{
			if (i != j) {
				li *= static_cast<OutputType>(value - this->supportingPoints[j].getFirst()) /
					  static_cast<OutputType>(this->supportingPoints[i].getFirst() -
							  this->supportingPoints[j].getFirst());
			}
		}
		ret += li * this->supportingPoints[i].getSecond();
	}

	return ret;
}
