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

#ifndef XPCC__FIR_HPP
#define XPCC__FIR_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	A finit impulse response (FIR) filter implementation
	 *
	 * g[n] = SUM(h[k]x[n-k])
	 * 
	 * \todo	
	 * 
	 * \author	Kevin Laeufer
	 * \ingroup	filter
	 */
	namespace filter
	{
		template<typename T, int N, int BLOCK_SIZE>
		class Fir
		{

		public:
			/**
			 * \param	coeff	array containing the coefficients
			 **/
			Fir(const T (&coeff)[N]);
		
			/**
			 * Reset the coefficients.
			 *
			 * \param	coeff	array containing the coefficients
			 **/
			void
			setCoefficients(const T (&coeff)[N]);
		
			/**
			 * \brief	Resets the tap buffer
			 */
			void
			reset();

			/**
			 * \brief	Appends new tap
			 */
			void
			append(const T& input);
		
			/**
			 * \brief	Calculates g[0]
			 * 
			 */
			void
			update();
		
			/**
			 * \brief	Returns g[0].
			 */
			inline const T&
			getValue() const
			{
				return output;
			}
		
		private:
			T output;	
			T taps[N+BLOCK_SIZE];
			T coefficients[N];
			int taps_index;
		};
	}
}

#include "fir_impl.hpp"

#endif // XPCC__FIR_HPP
