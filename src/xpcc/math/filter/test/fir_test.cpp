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

#include <xpcc/math/filter/fir.hpp>

#include "fir_test.hpp"


#ifdef TEST_FLOAT
	#define TAP_ZERO 0.0f
	#define TAP_A 0.1111f
	#define TAP_B 0.2222f
	#define TAP_C 0.3333f
	#define TAP_D 0.4444f
	#define TAP_E 0.5555f
#else
	#define TAP_ZERO 0
	#define TAP_A 46
	#define TAP_B 83
	#define TAP_C 03
	#define TAP_D 29
	#define TAP_E 83
#endif


void
FirTest::testFir()
{
	/* Delay Line */
	float delay_line_coeffs[5] = {0,0,0,0,1};
	int delay_line_taps[5] = {TAP_A,TAP_B,TAP_C,TAP_D,TAP_E};
	int delay_line_results[10] = {TAP_ZERO,TAP_ZERO,TAP_ZERO,TAP_ZERO,TAP_A,TAP_B,TAP_C,TAP_D,TAP_E,TAP_ZERO};
	testFilter<int, 5, 2, 10>(delay_line_coeffs, delay_line_taps, 5, delay_line_results);
}

/* Length of results array needs to be len(taps) + len(coeff) */
template<typename T, int N, int BLOCK_SIZE, unsigned int ScaleFactor>
void FirTest::testFilter(const float (&coeff)[N],
	 const T taps[], int taps_length, const T results[])
{
	xpcc::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor> filter(coeff);
	for(int i = 0; i < (taps_length + N); i++){
		if(i < taps_length)
			filter.append(taps[i]);
		else
			filter.append(0);
		filter.update();
		TEST_ASSERT_EQUALS(filter.getValue(), results[i]);
	} 
}
