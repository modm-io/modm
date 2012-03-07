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
 * $Id: lagrange.hpp 658 2011-12-03 10:18:19Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_INTERPOLATION__LAGRANGE_HPP
#define	XPCC_INTERPOLATION__LAGRANGE_HPP

#include <stdint.h>

#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/container/pair.hpp>
#include <xpcc/architecture/driver/accessor.hpp>

#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
{
	namespace interpolation
	{
		/**
		 * \brief	Lagrange Interpolation
		 * 
		 * Example:
		 * \code
		 * typedef xpcc::Pair<float, float> Point;
		 * 
		 * // interpolate x^2 over the range of 1 <= x <= 3
		 * Point points[3] =
		 * {
		 *     { 1, 1 },
		 *     { 2, 4 },
		 *     { 3, 9 }
		 * };
		 * 
		 * xpcc::interpolation::Lagrange<Point> value(points, 3);
		 * 
		 * ...
		 * float output = value.interpolate(1.5f);
		 * // output => 2.25;
		 * \endcode
		 * 
		 * \see http://en.wikipedia.org/wiki/Lagrange_interpolation
		 * 
		 * \warning	Only floating points types are allowed as second type of
		 * 			xpcc::Pair, otherwise the calculation will deliver wrong
		 * 			results!
		 * 
		 * \tparam	T	Any specialization of xpcc::Pair<> with a floating
		 * 				point type as second template argument.
		 * \tparam	Accessor	Accessor class. Can be xpcc::accessor::Ram,
		 * 						xpcc::accessor::Flash or any self defined
		 * 						accessor class.
		 * 						Default is xpcc::accessor::Ram.
		 * 
		 * \ingroup	interpolation
		 */
		template <typename T,
				  template <typename> class Accessor = ::xpcc::accessor::Ram>
		class Lagrange
		{
		public:
			typedef typename T::FirstType InputType;
			typedef typename T::SecondType OutputType;
			
			// WARNING:
			// Only floating point types are allowed as second type of xpcc::Pair
			// because the calculation will deliver wrong results otherwise!
			XPCC__STATIC_ASSERT(xpcc::ArithmeticTraits<OutputType>::isFloatingPoint, 
					"Only floating point types are allowed as second type of xpcc::Pair");
		public:
			/**
			 * \brief	Constructor
			 * 
			 * \param	supportingPoints	Supporting points of the curve.
			 * 								Needs to be an Array of xpcc::Pair<>.
			 * \param	numberOfPoints		length of \p supportingPoints
			 */
			Lagrange(Accessor<T> supportingPoints, uint8_t numberOfPoints);
			
			/**
			 * \brief	Perform a Lagrange-interpolation
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

#include "lagrange_impl.hpp"

#endif	// XPCC_INTERPOLATION__LAGRANGE_HPP

