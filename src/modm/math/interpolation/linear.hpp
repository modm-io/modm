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

#include <modm/utils/arithmetic_traits.hpp>
#include <modm/container/pair.hpp>
#include <modm/architecture/driver/accessor.hpp>

namespace modm
{
	namespace interpolation
	{
		/**
		 * \brief	Linear interpolation between supporting points
		 * 
		 * Example:
		 * \code
		 * // Definition of the supporting points. The first type is the
		 * // input type, the second the output type
		 * typedef modm::Pair<int8_t, int16_t> Point;
		 * 
		 * // Create a array of supporting points describing the curve.
		 * Point supportingPoints[6] =
		 * {
		 *     { 30, -200 },
		 *     { 50, 0 },
		 *     { 90, 50 },
		 *     { 150, 2050 },
		 *     { 200, 3000 },
		 *     { 220, 20000 }
		 * };
		 * 
		 * modm::interpolation::Linear<Point>
		 *         value(supportingPoints, 6);
		 * 
		 * ...
		 * 
		 * int8_t  a = 20;
		 * int16_t b = value.interpolate(a);
		 * \endcode
		 * 
		 * Example with supporting points read from flash:
		 * \code
		 * // Definition of a supporting point
		 * typedef modm::Pair<int8_t, int16_t> Point;
		 * 
		 * // Array of supporting points in flash
		 * FLASH_STORAGE(Point supportingPoints[6]) =
		 * {
		 *     { 30, -200 },
		 *     { 50, 0 },
		 *     { 90, 50 },
		 *     { 150, 2050 },
		 *     { 200, 3000 },
		 *     { 220, 20000 }
		 * };
		 * 
		 * // Create an interpolator object which reads the
		 * // supporting points from flash.
		 * modm::interpolation::Linear<Point, modm::accessor::Flash>
		 *         value(modm::accessor::asFlash(supportingPoints), 6);
		 * 
		 * ...
		 * 
		 * int8_t  a = 20;
		 * int16_t b = value.interpolate(a);
		 * \endcode
		 * 
		 * \tparam	T			Any specialization of modm::Pair<>
		 * \tparam	Accessor	Accessor class. Can be modm::accessor::Ram,
		 * 						modm::accessor::Flash or any self defined
		 * 						accessor class.
		 * 						Default is modm::accessor::Ram.
		 * 
		 * \ingroup	interpolation
		 */
		template <typename T,
				  template <typename> class Accessor = ::modm::accessor::Ram>
		class Linear
		{
		public:
			typedef typename T::FirstType InputType;
			typedef typename T::SecondType OutputType;
			
			typedef typename ArithmeticTraits< OutputType >::SignedType OutputSignedType;
			typedef typename ArithmeticTraits< OutputSignedType >::WideType WideType;
			
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
