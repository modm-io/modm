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

#ifndef	XPCC__LINEAR_INTERPOLATION_HPP
   #error "Don't include this file directly. Use linear_interpolation.hpp instead"
#endif	//XPCC__LINEAR_INTERPOLATION_HPP

namespace xpcc
{	
	template<typename T>
	LinearInterpolation<T>::LinearInterpolation(FlashPointer<T> points, uint8_t numPoints):
	points(points), numPoints(numPoints){
		
	}
	template<typename T>
	T
	LinearInterpolation<T>::interpolate(T value)
	{
		uint8_t i;
		
		T x1 = 0, x2 = 0;
		if(value <= points[0]) return points[1];
		if(value >= points[2*(numPoints-1)]) return points[2*(numPoints-1)+1];
		for (i = 0; i < numPoints ; i++) {
			x2 = points[i*2];
			if (x2 >= value)
				break;
			x1 = x2;
		}
		
		T t1 = points[i*2-1];
		T t2 = points[i*2+1];
		
		return ((value - x1) * (t2 - t1)) / (x2 - x1) + t1;
	}
}
