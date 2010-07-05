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
	#error	"Don't include this file directly, use 'point_set_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::PointSet2D<T>::PointSet2D(SizeType n) :
	points(n)
{
}

// ----------------------------------------------------------------------------
template <typename T>
typename xpcc::PointSet2D<T>::SizeType
xpcc::PointSet2D<T>::getNumberOfPoints() const
{
	return points.getSize();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::PointSet2D<T>::append(const Vector2D<T>& point)
{
	points.append(point);
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Vector2D<T>&
xpcc::PointSet2D<T>::operator [](SizeType index)
{
	return points[index];
}

template <typename T>
const xpcc::Vector2D<T>&
xpcc::PointSet2D<T>::operator [](SizeType index) const
{
	return points[index];
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::PointSet2D<T>::removeAll()
{
	points.removeAll();
}
