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

#include "vector2.hpp"

// this explicit namespace is needed here, otherwise we get an error about 
// "specialization of ... in different namespace"
namespace xpcc
{
	template<>
	int16_t
	Vector<int16_t, 2>::getLength() const
	{
		int32_t t;
		
		t = math::mul(this->x, this->x);
		t = math::mac(t, this-> y, this->y);
		
		return math::sqrt(t);
	}

	template<>
	int32_t
	Vector<int16_t, 2>::getLengthSquared() const
	{
		int32_t t;
		
		t = math::mul(this->x, this->x);
		t = math::mac(t,this-> y, this->y);
		
		return t;
	}
	
	template<>
	int32_t
	Vector<int16_t, 2>::dot(const xpcc::Vector<int16_t, 2>& other) const
	{
		int32_t t;
		
		t = math::mul(this->x, other.x);
		t = math::mac(t,this->y, other.y);
		
		return t;
	}
	
	// ------------------------------------------------------------------------
	template<>
	template<>
	Vector<double, 2>
	Vector<float, 2>::convert() const
	{
		return Vector<double, 2>(this->x, this->y);
	}
	
	template<>
	template<>
	Vector<float, 2>
	Vector<double, 2>::convert() const
	{
		return Vector<float, 2>(this->x, this->y);
	}
}
