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

#ifndef XPCC__POINT_SET_2D_HPP
#define XPCC__POINT_SET_2D_HPP

#include <xpcc/container/dynamic_array.hpp>
#include "vector_2d.hpp"

namespace xpcc
{
	/**
	 * \brief	Point set
	 * 
	 * Collection of points, represented by their corresponding vectors.
	 * Used for example to hold the result of a intersection-operation.
	 * 
	 * Based on the xpcc::DynamicArray class, therefore grows automatically
	 * if more space than currently allocated is needed. But because this
	 * is an expensive operation it should be avoid if possible.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template <typename T>
	class PointSet2D
	{
	public:
		typedef std::size_t SizeType;

	public:
		/**
		 * \brief	Constructs a set capable of holding n points (default = 2)
		 */
		PointSet2D(SizeType n = 2);
		
		/// Number of points contained in the set
		inline SizeType
		getNumberOfPoints() const;
		
		
		inline void
		append(const Vector2D<T>& point);

		inline Vector2D<T>&
		operator [](SizeType index);

		inline const Vector2D<T>&
		operator [](SizeType index) const;
		
		/**
		 * \brief	Remove all points
		 */
		inline void
		removeAll();
		
	protected:
		xpcc::DynamicArray< Vector2D<T> > points;
	};
}

#include "point_set_2d_impl.hpp"

#endif // XPCC__POINT_SET_2D_HPP
