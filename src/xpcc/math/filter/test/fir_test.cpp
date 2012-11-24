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

#define TAP_ZERO 0.0f
#define TAP_A 0.8645f
#define TAP_B 0.7855f
#define TAP_C 0.2658f
#define TAP_D 0.0256f
#define TAP_E -0.456f


void
FirTest::testFir()
{
	/* Delay Line */
	float delay_line_coeffs[3] = {0,0,1};
	float delay_line_taps[5] = {TAP_A,TAP_B,TAP_C,TAP_D,TAP_E};
	float delay_line_results[8] = {TAP_ZERO,TAP_ZERO,TAP_ZERO,TAP_A,TAP_B,TAP_C,TAP_D,TAP_E};
	testFilter<float, 3>(delay_line_coeffs, delay_line_taps, 5, delay_line_results);

	/*

	
	xpcc::filter::Fir<float, 3> filter(delay_line_coeffs);

	// Append two new taps
	filter.append(TAP_A);
	filter.append(TAP_C);

	// Result should be Zero
	filter.update();
	TEST_ASSERT_EQUALS(filter.getValue(), TAP_ZERO);

	// Append third Tap
	filter.append(TAP_B);

	// Result should now be TAP_A
	filter.update();
	TEST_ASSERT_EQUALS(filter.getValue(), TAP_A);

	filter.append(TAP_B);
	filter.update();
	TEST_ASSERT_EQUALS(filter.getValue(), TAP_C);
	*/
}

/* Length of results array needs to be len(taps) + len(coeff) */
template<typename T, int N>
void FirTest::testFilter(const T (&coeff)[N],
	 const T taps[], int taps_length, const T results[])
{
	xpcc::filter::Fir<T, N> filter(coeff);
	for(int i = 0; i < (taps_length + N); i++){
		filter.update();
		TEST_ASSERT_EQUALS(filter.getValue(), results[i]);
		if(i < taps_length)
			filter.append(taps[i]);
		else
			filter.append(0);
	} 
}
