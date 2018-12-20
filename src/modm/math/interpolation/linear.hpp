/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, 2013, Fabian Greif
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
#define	MODM_INTERPOLATION_LINEAR_HPP

#include <stdint.h>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/container/pair.hpp>
#include <modm/architecture/interface/accessor.hpp>

namespace modm
{
	namespace interpolation
	{
		/**
		 * \tparam	T			Any specialization of modm::Pair<>
		 * \tparam	Accessor	Accessor class. Can be modm::accessor::Ram,
		 * 						modm::accessor::Flash or any self defined
		 * 						accessor class.
		 * 						Default is modm::accessor::Ram.
		 *
		 * \ingroup	modm_math_interpolation
		 */
		template <typename T,
				  template <typename> class Accessor = ::modm::accessor::Ram>
		class Linear
		{
		public:
			typedef typename T::FirstType InputType;
			typedef typename T::SecondType OutputType;

			typedef modm::SignedType< OutputType > OutputSignedType;
			typedef modm::WideType< OutputSignedType > WideType;

		public:
			/**
			 * \brief	Constructor
			 *
			 * \param	supportingPoints	Supporting points of the curve.
			 * 								Needs to be an Array of modm::Pair<>.
			 * \param	numberOfPoints		length of \p supportingPoints
			 */
			Linear(Accessor<T> supportingPoints, uint8_t numberOfPoints);

			/**
			 * \brief	Perform a linear interpolation
			 *
			 * \param 	value	input value
			 * \return	interpolated value
			 */
			OutputType
			interpolate(const InputType& value) const;

		private:
			const Accessor<T> supportingPoints;
			const uint8_t numberOfPoints;
		};
	}
}

#include "linear_impl.hpp"

#endif	// MODM_INTERPOLATION_LINEAR_HPP
