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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__ANGLE_HPP
#define	XPCC__ANGLE_HPP

#include <cmath>
#include <math.h>

// The circumference of a circle with diameter 1
#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

#ifndef M_TWOPI
#define M_TWOPI (M_PI * 2.0)
#endif

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154
#endif

#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308
#endif

// The square root of 2.
#ifndef M_SQRT2 
#define M_SQRT2  1.41421356237309504880 
#endif 

namespace xpcc
{
	/**
	 * \brief	Collection of functions for handling of angles
	 * 
	 * Angles are always represented by float values in the range
	 * from -Pi to Pi.
	 * 
	 * \ingroup	geometry
	 */
	class Angle
	{
	public:
		typedef float Type;
		
		/**
		 * \brief	Normalize angle
		 * 
		 * Normalize the given angle to [-Pi,Pi] by repeatedly
		 * adding/subtracting 2*Pi.
		 */
		static float
		normalize(float angle);
		
		/**
		 * \brief	Reverse the angle
		 * 
		 * Reverse the angle and keep it normalized to [-Pi,Pi].
		 * 
		 * Equivalent to:
		 * \code
		 * float angle = xpcc::Angle::normalize(angle + M_PI);
		 * \endcode
		 */
		static float
		reverse(float angle);
		
		/**
		 * \brief	Find a perpendicular angle
		 *
		 * \param	angle
		 * \param	cw	If cw is true the angle is rotated clockwise.
		 * 				Ohterwise the angle is rotated anti clockwise.
		 */

		static float
		perpendicular(float angle, const bool cw);
		
		static inline float
		toRadian(float angle)
		{
			return (angle * M_PI) / 180.0;
		}
		
		static inline float
		toDegree(float angle)
		{
			return (angle * 180.0) / M_PI;
		}
	};
}

#endif	// XPCC__ANGLE_HPP
