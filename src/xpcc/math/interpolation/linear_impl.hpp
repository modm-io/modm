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

#ifndef	XPCC_INTERPOLATION__LINEAR_HPP
   #error "Don't include this file directly. Use 'xpcc/math/interpolation/linear.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
xpcc::interpolation::Linear<T, Accessor>::Linear(
		Accessor<T> supportingPoints, uint8_t numberOfPoints) :
	supportingPoints(supportingPoints), numberOfPoints(numberOfPoints)
{
}
	
// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
typename xpcc::interpolation::Linear<T, Accessor>::OutputType
xpcc::interpolation::Linear<T, Accessor>::interpolate(const InputType& value) const
{
	T current(this->supportingPoints[0]);
	
	if (value <= current.getFirst()) {
		return current.getSecond();
	}
	
	T last(current);
	uint8_t i;
	for (i = 1; i < this->numberOfPoints; ++i)
	{
		current = this->supportingPoints[i];
		
		if (value <= current.getFirst())
		{
			InputType x1_in = last.getFirst();
			InputType x2_in = current.getFirst();
			
			OutputType x1_out = last.getSecond();
			OutputType x2_out = current.getSecond();
			
			InputType a = value - x1_in;		// >0
			WideType b = static_cast<OutputSignedType>(x2_out) - 
						 static_cast<OutputSignedType>(x1_out);
			InputType c = x2_in - x1_in;		// >0
			
			return static_cast<OutputType>(((a * b) / c) + x1_out);
		}
		
		last = current;
	}
	
	return current.getSecond();
}
