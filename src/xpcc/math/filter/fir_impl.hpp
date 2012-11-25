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

#define likely(x) __builtin_expect((x),1)

//#define DEBUG

#ifdef DEBUG
	#include <stdio.h>

	#define DEBUG_APPEND
	#define DEBUG_UPDATE

	#ifdef DEBUG_UPDATE
	#define DEBUG_SUM(tap, coeff) printf("sum += %.3f * %.3f;\n", tap, coeff)
	#else
	#define DEBUG_SUM(tap, coeff)
	#endif /*DEBUG_UPDATE*/
#else
	#define DEBUG_SUM(tap, coeff)
#endif /*DEBUG*/



template<typename T, int N, int BLOCK_SIZE>
xpcc::filter::Fir<T, N, BLOCK_SIZE>::Fir(const T (&coeff)[N])
{
	setCoefficients(coeff);
	reset();
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE>
void
xpcc::filter::Fir<T, N, BLOCK_SIZE>::setCoefficients(const T (&coeff)[N])
{
	for(int i = 0; i < N; i++){
		coefficients[i] = coeff[i];
	}
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE>
void
xpcc::filter::Fir<T, N, BLOCK_SIZE>::reset()
{
	// reset all to make debugging easier
	for(int i = 0; i < N + BLOCK_SIZE; i++){
		taps[i] = (T)0;
	}
	taps_index = BLOCK_SIZE;
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE>
void
xpcc::filter::Fir<T, N, BLOCK_SIZE>::append(const T& input)
{
#ifdef DEBUG_APPEND
	printf("append(%f);\ttaps_index=%d\n", input, taps_index);
	printf("BEFORE: ");
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n");
#endif // DEBUG_APPEND
	if(likely(taps_index > 0)){
		taps_index--;
	}
	else{
		for(int i = N + BLOCK_SIZE - 1; i > BLOCK_SIZE; i--)
			taps[i] = taps[i - BLOCK_SIZE - 1];
		taps_index = BLOCK_SIZE;
	}
	taps[taps_index] = input;

#ifdef DEBUG_APPEND
	printf(" AFTER: ");
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n\n");
#endif // DEBUG_APPEND
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE>
void
xpcc::filter::Fir<T, N, BLOCK_SIZE>::update()
{
#ifdef DEBUG_UPDATE
	printf("update();\ttaps_index=%d\n", taps_index);
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n");
#endif // DEBUG_UPDATE

	T sum = (T)0;
	T *tap = taps + taps_index;
	for(int i = 0; i < (N - (N%4)); i++){
		DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i];
	}
	for(int i = (N - (N%4)); i < N; i++){
		DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i];
	}
	output = sum;
#ifdef DEBUG_UPDATE
	printf("sum=%.3f\n", sum);
#endif // DEBUG_UPDATE
}
#endif // XPCC__FIR_IMPL_HPP
