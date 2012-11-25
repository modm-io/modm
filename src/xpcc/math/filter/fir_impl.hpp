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

#ifndef XPCC__FIR_IMPL_HPP
#define XPCC__FIR_IMPL_HPP

template<typename T, int N>
xpcc::filter::Fir<T, N>::Fir(const T (&coeff)[N])
{
	setCoefficients(coeff);
	reset();
}

// -----------------------------------------------------------------------------
template<typename T, int N>
void
xpcc::filter::Fir<T, N>::setCoefficients(const T (&coeff)[N])
{
	for(int i = 0; i < N; i++){
		coefficients[i] = coeff[i];
	}
}

// -----------------------------------------------------------------------------
template<typename T, int N>
void
xpcc::filter::Fir<T, N>::reset()
{
	for(int i = 0; i < N; i++){
		taps[i] = (T)0;
	}
}

// -----------------------------------------------------------------------------
template<typename T, int N>
void
xpcc::filter::Fir<T, N>::append(const T& input)
{
	for(int i = N-1; i > 0; i--){
		taps[i] = taps[i-1];
	}
	taps[0] = input;
}

// -----------------------------------------------------------------------------
template<typename T, int N>
void
xpcc::filter::Fir<T, N>::update()
{
	T sum = (T)0;
	for(int i = 0; i < (N - (N%4)); i++){
		sum += taps[i]*coefficients[i]; i++;
		sum += taps[i]*coefficients[i]; i++;
		sum += taps[i]*coefficients[i]; i++;
		sum += taps[i]*coefficients[i];
	}
	for(int i = (N - (N%4)); i < N; i++){
		sum += taps[i]*coefficients[i];
	}
	output = sum;
}
#endif // XPCC__FIR_IMPL_HPP
