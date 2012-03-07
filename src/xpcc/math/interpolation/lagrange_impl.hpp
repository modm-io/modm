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
 * $Id: lagrange_impl.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_INTERPOLATION__LAGRANGE_HPP
   #error "Don't include this file directly. Use 'xpcc/math/interpolation/lagrange.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T,
		  template <typename> class Accessor>
xpcc::interpolation::Lagrange<T, Accessor>::Lagrange(
		Accessor<T> supportingPoints, uint8_t numberOfPoints) :
	supportingPoints(supportingPoints), numberOfPoints(numberOfPoints)
{
}

// ----------------------------------------------------------------------------

template <typename T,
		  template <typename> class Accessor>
typename xpcc::interpolation::Lagrange<T, Accessor>::OutputType
xpcc::interpolation::Lagrange<T, Accessor>::interpolate(const InputType& value) const
{
	OutputType ret = 0;
	for (uint8_t i = 0; i < this->numberOfPoints; ++i)
	{
		OutputType li = 1;
		for (uint8_t j = 0; j < this->numberOfPoints; ++j)
		{
			if (i != j) {
				li *= static_cast<OutputType>(value - this->supportingPoints[j].getFirst()) / 
					  static_cast<OutputType>(this->supportingPoints[i].getFirst() - 
							  this->supportingPoints[j].getFirst());
			}
		}
		ret += li * this->supportingPoints[i].getSecond();
	}
	
	return ret;
}
