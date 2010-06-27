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

#ifndef XPCC__LINE_SEGMENT_2D_HPP
	#error	"Don't include this file directly, use 'line_segment_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::LineSegment2D<T>::LineSegment2D() :
	Line2D<T>()
{
}

template<typename T>
xpcc::LineSegment2D<T>::LineSegment2D(const Point2D<T>& start, const Point2D<T>& end) :
	Line2D<T>(start, Vector2D<T>(start, end))
{
}

template<typename T>
T
xpcc::LineSegment2D<T>::getLength() const
{
	return this->vector.getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
const T
xpcc::LineSegment2D<T>::getDistanceTo(const Point2D<T>& point) const
{
	// vector from the base point of the line to the new point
	Vector2D<T> w(this->point, point);
	
	float c1 = Vector2D<T>::dot(w, this->vector);
	if (c1 <= 0) {
		return w.getLength();
	}
	
	float c2 = Vector2D<T>::dot(this->vector, this->vector);
	if (c2 <= c1) {
		// TODO
		//Vector2D<T> p();
		//return p.getLength();
	}
	
	float d = c1 / c2;
	
	Vector2D<T> p(this->point);
	p.scale(d);
	
	return p.getLength();
}

// ----------------------------------------------------------------------------
template<typename U>
bool
xpcc::operator == (const LineSegment2D<U> &a, const LineSegment2D<U> &b)
{
	return ((a.start == b.start) &&
			(a.end == b.end));
}

template<typename U>
bool
xpcc::operator != (const LineSegment2D<U> &a, const LineSegment2D<U> &b)
{
	return ((a.start != b.start) ||
			(a.end != b.end));
}
