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

#ifndef	XPCC__ANGLE_HPP
#define	XPCC__ANGLE_HPP

#include <cmath>
#include <xpcc/architecture/platform.hpp>

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
