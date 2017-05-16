/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Kevin Läufer
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/filter/fir.hpp>

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
	modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor> filter(coeff);
	for(int i = 0; i < (taps_length + N); i++){
		if(i < taps_length)
			filter.append(taps[i]);
		else
			filter.append(0);
		filter.update();
		TEST_ASSERT_EQUALS(filter.getValue(), results[i]);
	} 
}
