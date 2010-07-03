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

#include <xpcc/math/utils/operator.hpp>

#include "vector_2d.hpp"

// this explicit namespace is needed here, otherwise we get an error about 
// "specialization of ... in different namespace"
namespace xpcc
{
	template<>
	Vector2D<float>&
	Vector2D<float>::rotate(const float phi)
	{
		float c = std::cos(phi);
		float s = std::sin(phi);

		float tx = (c * this->x - s * this->y);
		this->y =  (s * this->x + c * this->y);
		this->x = tx;

		return *this;
	}
	
	// ------------------------------------------------------------------------
	template<>
	float
	Vector2D<float>::getLength() const
	{
		return std::sqrt(this->x * this->x +
						 this->y * this->y);
	}
	
	template<>
	double
	Vector2D<double>::getLength() const
	{
		return std::sqrt(this->x * this->x +
						 this->y * this->y);
	}
	
	// ------------------------------------------------------------------------
	template<>
	int16_t
	Vector2D<int16_t>::getLength() const
	{
		int32_t t;
		t = math::mul32(x, x);
		t = math::mac32(t, y, y);
		
		return math::sqrt32(t);
	}
	
	template<>
	int32_t
	Vector2D<int16_t>::getLengthSquared() const
	{
		int32_t t;
		
		t = math::mul32(x, x);
		t = math::mac32(t, y, y);
		
		return t;
	}
	
	// ------------------------------------------------------------------------
	template<>
	template<>
	Vector2D<double>
	Vector2D<float>::convert() const
	{
		return Vector2D<double>(this->x, this->y);
	}
	
	template<>
	template<>
	Vector2D<float>
	Vector2D<double>::convert() const
	{
		return Vector2D<float>(this->x, this->y);
	}
}
