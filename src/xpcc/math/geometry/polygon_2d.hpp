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

#ifndef XPCC__POLYGON_2D_HPP
#define XPCC__POLYGON_2D_HPP

#include <xpcc/container/dynamic_array.hpp>
#include "point_2d.hpp"

namespace xpcc
{
	/**
	 * \brief	Polygon
	 *
	 * The Polygon class provides a vector of points.
	 *
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template <typename T>
	class Polygon2D
	{
	public:
		typedef std::size_t SizeType;

	public:
		/**
		 * \brief	Constructs a polygon capable of holding n points
		 */
		Polygon2D(SizeType n);

		void
		append(const Point2D<T>& point);

		inline Point2D<T>&
		operator [](SizeType index);

		inline const Point2D<T>&
		operator [](SizeType index) const;

	private:
		xpcc::DynamicArray<T> buffer;
	};
}

#include "polygon_2d_impl.hpp"

#endif // XPCC__POLYGON_2D_HPP
